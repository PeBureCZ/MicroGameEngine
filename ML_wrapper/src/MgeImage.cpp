#include "MgeImage.h"

#include "MlWrapper.h"

MgeImage::MgeImage(TextureId usedTextureId, size_t usedLayer, FPoint newPosition)
	: usedTexture(std::move(usedTextureId)), layer(usedLayer)
{
	imageId = reinterpret_cast<VerticesId>(this);
	const sf::Texture& usedSfTexture = ML_wrapper::getGlobalMlWrapper()->appendTexture(*this);
	absolutePosition = newPosition;
	layer = usedLayer;
	sprite = std::make_shared<sf::Sprite>(usedSfTexture);
	size = mgeType::Size<int>((int)usedSfTexture.getSize().x, (int)usedSfTexture.getSize().y);
	ML_wrapper::getGlobalMlWrapper()->appendImage(*this);
}

MgeImage::MgeImage()
{
	imageId = reinterpret_cast<VerticesId>(this);
}

MgeImage::MgeImage(const MgeImage& other)
{
	imageId = reinterpret_cast<VerticesId>(this);
	sf::Texture usedSfTexture = ML_wrapper::getGlobalMlWrapper()->appendTexture(*this);
	usedTexture = other.usedTexture;
	absolutePosition = other.absolutePosition;
	layer = other.layer;
	size = other.size;
	sprite = std::make_shared<sf::Sprite>(usedSfTexture);
	ML_wrapper::getGlobalMlWrapper()->appendImage(*this);
}

MgeImage& MgeImage::operator=(const MgeImage& other)
{
	if (this == &other)
		return *this;
	imageId = reinterpret_cast<VerticesId>(this);
	size = other.size;
	usedTexture = other.usedTexture;
	absolutePosition = other.absolutePosition;
	layer = other.layer;
	ML_wrapper::getGlobalMlWrapper()->appendImage(*this);
	return *this;
}

mgeType::Size<int> MgeImage::getSize() const noexcept
{
	_ASSERT(size.width != 0 && size.height != 0);
	return size;
}

MgeImage::~MgeImage()
{
	if (sprite)
	{
		_ASSERT(usedTexture.getPath() != UNDEFINED_TEXTURE_PATH && !usedTexture.getPath().empty());
		ML_wrapper::getGlobalMlWrapper()->removeImage(*this);
		_ASSERT(sprite.use_count() == 1); //correct = sprite live in MgeImage only, not in any layer or another object
	}
	sprite.reset();
}

SPRITE_ID MgeImage::getSpriteID() const noexcept
{
    return imageId;
}

TextureId MgeImage::getTextureID() const noexcept
{
	return usedTexture;
}

const std::shared_ptr<sf::Sprite> MgeImage::getSprite() const noexcept
{
	_ASSERT(sprite);
	return sprite;
}

void MgeImage::setColor(const mgeType::Color_RGBA& newColor) noexcept
{
	_ASSERT(sprite);
	if (sprite)
	{
		m_alpha = newColor.a;
		sprite->setColor(sf::Color(newColor.r, newColor.g, newColor.b, m_isVisible ? newColor.a : 0));
	}
}

void MgeImage::setVisible(bool visible)
{
	m_isVisible = visible;
	_ASSERT(sprite);
	if (sprite)
	{
		auto color = sprite->getColor();
		color.a = visible ? m_alpha : 0;
		sprite->setColor(color);
	}
}

void MgeImage::setOrigin(const FPoint& newOrigin) const
{
	_ASSERT(sprite);
	if (sprite)
		sprite->setOrigin(sf::Vector2f(newOrigin.x, newOrigin.y));
}

void MgeImage::setRotation(float newRotation) const
{
	_ASSERT(sprite); 
	if (sprite)
		sprite->setRotation(sf::degrees(newRotation));
}

float MgeImage::getRotation() const
{
	_ASSERT(sprite);
	if (sprite)
		return sprite->getRotation().asDegrees();
	return 0.f;
}

FPoint MgeImage::getAbsolutePosition() const noexcept
{
	return absolutePosition;
}

size_t MgeImage::getLayer() const noexcept
{
	return layer;
}

void MgeImage::setLayer(size_t newLayer)
{
	layer = newLayer;
	ML_wrapper::getGlobalMlWrapper()->changeSpriteLayer(sprite, newLayer, layer);
}

void  MgeImage::setImgAbsolutePosition(FPoint newPosition)
{
	_ASSERT(sprite);
	if (sprite)
	{
		absolutePosition = newPosition;
		sprite->setPosition(sf::Vector2f(newPosition.x, newPosition.y));
	}
}

MgeImage MgeImage::loadTexture(std::string path, size_t layer)
{
	return MgeImage(TextureId(path), layer);
}

MgeImage MgeImage::loadTexture(const TextureId& textureId, size_t layer)
{
	return MgeImage(TextureId(textureId.getPath()), layer);
}
