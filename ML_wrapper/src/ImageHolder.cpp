#include "ImageHolder.h"

#include "GraphicDependencies.h"

void ImageHolder::removeTexture(TextureId textureId)
{
	// decrease reference count
	auto texIt = textures.find(textureId);
	if (texIt != textures.end())
	{
		texIt->second.first--;

		if (texIt->second.first == 0)
		{
			// nobody uses this texture anymore → remove it
			textures.erase(texIt);
		}
	}
	else
	{
		_ASSERT(false); // texture id does not exist
	}
}

TextureId ImageHolder::appendTexture(const MlImage& image)
{
	auto textureId = image.getTextureID();
	if (textures.contains(textureId))
		textures.find(textureId)->second.first++; //increase counter
	sf::Texture texture;
	if (!texture.loadFromFile(textureId.path))
	{
		_ASSERT(false); //file not found
	}
	else
		textures.insert({ textureId, std::make_pair(1, texture) });
	return textureId;
}

ImageHolder::ImageHolder()
{

}

void ImageHolder::appendGuiImage(const MlImage& image, FPoint position)
{
	appendImage(image, GraphicItemLayer::GUI_LAYER, position);
}

void ImageHolder::removeGuiImage(TextureId textureId, SPRITE_ID spriteId)
{
	removeImage(std::move(textureId), spriteId, GraphicItemLayer::GUI_LAYER);
}

void ImageHolder::setImageAbsolutePosition(SPRITE_ID image, size_t layer, FPoint newPosition)
{
	if (sprites.find(layer) != sprites.end())
	{
		// layer exists
		auto it = sprites[layer].find(image);
		if (it != sprites[layer].end())
			it->second.setPosition(sf::Vector2f(newPosition.x, newPosition.y));
		else
		{
			_ASSERT(false); //wrong image management
		}
	}
	else
	{
		_ASSERT(false); //wrong layer management
	}
}

void ImageHolder::setSpriteColor(SPRITE_ID image, mgeType::Color_RGBA newColor, size_t layer)
{
	auto findedLayer = sprites.find(layer);
	if (findedLayer != sprites.end())
	{
		auto it = findedLayer->second.find(image);
		if (it != findedLayer->second.end())
			it->second.setColor(sf::Color(newColor.r, newColor.g, newColor.b, newColor.a));
	}
	else
	{
		_ASSERT(false); //wrong sprite management
	}
}

[[nodiscard]] mgeType::Color_RGBA ImageHolder::getSpriteColor(SPRITE_ID image, size_t layer)
{
	auto findedLayer = sprites.find(layer);
	if (findedLayer != sprites.end())
	{
		auto it = findedLayer->second.find(image);
		if (it != findedLayer->second.end())
		{
			sf::Color color = it->second.getColor();
			return mgeType::Color_RGBA(color.r, color.g, color.b, color.a);
		}
		else
		{
			_ASSERT(false); //wrong image management
			return mgeType::Color_RGBA();
		}
	}
	else
	{
		_ASSERT(false); //wrong image management
		return mgeType::Color_RGBA();
	}
}

void ImageHolder::changeSpriteLayer(const TextureId& textureId, SPRITE_ID imageId, size_t newLayer, size_t oldLayer)
{
	FPoint oldPosition;
	auto findedLayer = sprites.find(oldLayer);
	if (findedLayer != sprites.end())
	{
		auto it = findedLayer->second.find(imageId);
		if (it != findedLayer->second.end())
		{
			auto spritePos = it->second.getPosition();
			oldPosition = FPoint(spritePos.x, spritePos.y);
		}
	}

	MlImage image(textureId, GraphicItemLayer::GUI_LAYER, oldPosition);
	removeImage(textureId, imageId, oldLayer);
	appendImage(image, newLayer, oldPosition);
}

[[nodiscard]] mgeType::Size<int> ImageHolder::getTextureSize(TextureId id)
{
	for (const auto&texture : textures)
	{
		if (texture.first.path == id.path)
		{
			sf::Vector2u texSize = texture.second.second.getSize();
			static mgeType::Size<int> size;
			size.width = static_cast<int>(texSize.x);
			size.height = static_cast<int>(texSize.y);
			return size;
		}
	}

	_ASSERT(false); //texture not found - maybe not loaded yet
	return mgeType::Size<int>();
}

void ImageHolder::setSpriteRotation(SPRITE_ID image, float newRotation, size_t layer)
{
	auto findedLayer = sprites.find(layer);
	if (findedLayer != sprites.end())
	{
		auto it = findedLayer->second.find(image);
		if (it != findedLayer->second.end())
			it->second.setRotation(sf::Angle(sf::degrees(newRotation)));
	}
}

[[nodiscard]] float ImageHolder::getSpriteRotation(SPRITE_ID image, size_t layer) const
{
	auto layerIt = sprites.find(layer);
	if (layerIt == sprites.end())
	{
		_ASSERT(false); //wrong image management
		return 0.0f;
	}

	const auto& sprites = layerIt->second;
	auto spriteIt = sprites.find(image);
	if (spriteIt == sprites.end())
	{
		_ASSERT(false); //wrong image management
		return 0.0f;
	}

	return spriteIt->second.getRotation().asDegrees();
}

void ImageHolder::appendImage(const MlImage& image, size_t layer, FPoint position)
{
	auto textureId = appendTexture(image);
	auto sprite = sf::Sprite(textures[textureId.path].second);
	sprite.setPosition(sf::Vector2f(static_cast<float>(position.x), static_cast<float>(position.y)));

	if (textures.contains(textureId.path))
		textures.find(textureId.path)->second.first++; //increase counter
	sf::Texture texture;
	if (!texture.loadFromFile(textureId.path))
	{
		_ASSERT(false); //file not found
	}
	else
		textures.insert({ textureId.path, std::make_pair(1, texture) });

	//addSprite
	sprites[layer].insert({ image.getSpriteID(), std::move(sprite)});
}

void ImageHolder::removeImage(TextureId textureId, SPRITE_ID spriteId, size_t layer)
{
	_ASSERT(textureId.getPath() != UNDEFINED_TEXTURE_PATH);

	auto spriteIt = sprites.find(layer);
	bool continueToRemove = true;
	if (spriteIt == sprites.end())
		{
		_ASSERT(false); //wrong layer management
		continueToRemove = false;
	}
	else
	{ //correct layer found, now check if sprite exists
		auto spriteLayerIt = sprites[layer].find(spriteId);
		if (spriteLayerIt == sprites[layer].end())
		{ //correct layer found, but sprite not found
			_ASSERT(false); //wrong image management
			continueToRemove = false;
		}
	}

	if (continueToRemove)
		sprites[layer].erase(spriteId);

	removeTexture(std::move(textureId));
}

[[nodiscard]] const std::optional<const SPRITES*> ImageHolder::getGuiSprites() const
{
	auto it = sprites.find(GraphicItemLayer::GUI_LAYER);
	if (it != sprites.end())
		return &(it->second);
	return std::nullopt;
}

[[nodiscard]] const LAYERED_SPRITES& ImageHolder::getSprites() const noexcept
{
	return sprites;
}

ImageHolder::~ImageHolder()
{
	textures.clear();
}
