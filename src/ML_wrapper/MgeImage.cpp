#include "MgeImage.h"

#include "MlWrapper.h"
#include "GlobalFunctions.h"

MgeImage::MgeImage(TextureId usedTextureId, size_t usedLayer, FPoint newPosition)
	: usedTexture(std::move(usedTextureId))
{
	imageId = reinterpret_cast<VerticesId>(this);

	const sf::Texture& usedSfTexture = ML_wrapper::getGlobalMlWrapper()->appendTexture(*this);
	auto newSprite = std::make_shared<MgeLayerObject>(MgeLayerObject{ mgeCore::getDefaultZPosition(), usedLayer, sf::Sprite{usedSfTexture} });
	ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newSprite);
	if (newSprite)
		m_sprite = newSprite;

	absolutePosition = newPosition;
	size = mgeType::Size<int>((int)usedSfTexture.getSize().x, (int)usedSfTexture.getSize().y);
}

MgeImage::MgeImage()
{
	imageId = reinterpret_cast<VerticesId>(this);
}

MgeImage::MgeImage(const MgeImage& other)
{
	imageId = reinterpret_cast<VerticesId>(this);
	usedTexture = other.usedTexture;
	absolutePosition = other.absolutePosition;
	size = other.size;
	const sf::Texture& usedSfTexture = ML_wrapper::getGlobalMlWrapper()->appendTexture(*this);
	auto newSprite = std::make_shared<MgeLayerObject>(MgeLayerObject{ mgeCore::getDefaultZPosition(), other.getLayer(), sf::Sprite{usedSfTexture}});
	ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newSprite);
	if (newSprite)
		m_sprite = newSprite;

}

MgeImage& MgeImage::operator=(const MgeImage& other)
{
	if (this == &other)
		return *this;
	imageId = reinterpret_cast<VerticesId>(this);
	size = other.size;
	usedTexture = other.usedTexture;
	absolutePosition = other.absolutePosition;
	const sf::Texture& usedSfTexture = ML_wrapper::getGlobalMlWrapper()->appendTexture(*this);
	auto newSprite = std::make_shared<MgeLayerObject>(MgeLayerObject{ mgeCore::getDefaultZPosition(), other.getLayer(), sf::Sprite{usedSfTexture} });
	ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newSprite);
	return *this;
}

mgeType::Size<int> MgeImage::getSize() const noexcept
{
	_ASSERT(size.width != 0 && size.height != 0);
	return size;
}

SPRITE_ID MgeImage::getSpriteID() const noexcept
{
    return imageId;
}

TextureId MgeImage::getTextureID() const noexcept
{
	return usedTexture;
}

const std::shared_ptr<MgeLayerObject> MgeImage::getSprite() const noexcept
{
	_ASSERT(!m_sprite.expired());
	return m_sprite.lock();
}

void MgeImage::setColor(const mgeType::Color_RGBA& newColor) noexcept
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		m_alpha = newColor.a;
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			std::get<sf::Sprite>(sprite->data).setColor(sf::Color(newColor.r, newColor.g, newColor.b, m_isVisible ? newColor.a : 0));
		}
		else
			_ASSERT(false);
	}
}

void MgeImage::setVisible(bool visible)
{
	m_isVisible = visible;
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			auto& sfSprite = std::get<sf::Sprite>(sprite->data);
			auto color = sfSprite.getColor();
			color.a = visible ? m_alpha : 0;
			sfSprite.setColor(color);
		}
		else
			_ASSERT(false);
	}
}

void MgeImage::setOrigin(const FPoint& newOrigin) const
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			auto& sfSprite = std::get<sf::Sprite>(sprite->data);
			sfSprite.setOrigin(sf::Vector2f(newOrigin.x, newOrigin.y));
		}
		else
			_ASSERT(false);
	}
}

void MgeImage::setRotation(float newRotation) const
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			auto& sfSprite = std::get<sf::Sprite>(sprite->data);
			sfSprite.setRotation(sf::degrees(newRotation));
		}
		else
			_ASSERT(false);
	}
}

float MgeImage::getRotation() const
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			auto& sfSprite = std::get<sf::Sprite>(sprite->data);
			return sfSprite.getRotation().asDegrees();
		}
		else
			_ASSERT(false);
	}
	return 0.f;
}

FPoint MgeImage::getAbsolutePosition() const noexcept
{
	return absolutePosition;
}

size_t MgeImage::getLayer() const noexcept
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
		sprite->m_layer;
	return 0;
}

void MgeImage::setLayer(size_t newLayer)
{
	MAIN_THREAD_GUARD;
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
		ML_wrapper::getGlobalMlWrapper()->changeLayerOfMgeObject(sprite, newLayer);
}

void  MgeImage::setImgAbsolutePosition(FPoint newPosition)
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		absolutePosition = newPosition;
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			auto& sfSprite = std::get<sf::Sprite>(sprite->data);
			sfSprite.setPosition(sf::Vector2f(newPosition.x, newPosition.y));
		}
		else
			_ASSERT(false);
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

void MgeImage::setZPosition(const int64_t newZPosition) noexcept
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		sprite->zPosition = newZPosition;
		ML_wrapper::getGlobalMlWrapper()->sortLayerByZIndex_delayed(getLayer());
	}
}

int64_t MgeImage::getZPosition() const noexcept
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
		return sprite->zPosition;
	return 0;
}

MgeImage::~MgeImage()
{
	if (auto sprite = m_sprite.lock())
	{
		_ASSERT(usedTexture.getPath() != UNDEFINED_TEXTURE_PATH && !usedTexture.getPath().empty());
		ML_wrapper::getGlobalMlWrapper()->removeMgeLayerObject(sprite);
		m_sprite.reset();
		_ASSERT(sprite.use_count() == 1); //correct = last instance is local
	}
}
