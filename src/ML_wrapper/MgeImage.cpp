#include "MgeImage.h"

#include "MlWrapper.h"
#include "GlobalFunctions.h"
#include <filesystem>

namespace
{
	std::string generateExtension(MgeImage::ImgType type)
	{
		switch (type)
		{
		case MgeImage::ImgType::BMP: return { ".bmp" };
		case MgeImage::ImgType::PNG: return { ".png" };
		case MgeImage::ImgType::JPG: return { ".jpg" };
		case MgeImage::ImgType::TGA: return { ".tga" };
		case MgeImage::ImgType::GIF: return { ".gif" };
		case MgeImage::ImgType::PSD: return { ".psd" };
		case MgeImage::ImgType::HDR: return { ".hdr" };
		case MgeImage::ImgType::PIC: return { ".pic" };
		case MgeImage::ImgType::PNM: return { ".pnm" };
		default: _ASSERT(false); //undefined or unknown type
		}
		return { ".png" };
	}

	std::string m_defaultPath;
	std::string defaultExtension = ".png";
}

MgeImage::MgeImage(const TextureId& usedTextureId, size_t usedLayer, const FPoint& newPosition)
	: m_usedTexture(usedTextureId)
{
	imageId = reinterpret_cast<VerticesId>(this);
	absolutePosition = newPosition;
	createNewSprite(usedLayer);
}

MgeImage::MgeImage()
{
	imageId = reinterpret_cast<VerticesId>(this);
}

MgeImage::MgeImage(const MgeImage& other)
{
	imageId = reinterpret_cast<VerticesId>(this);
	m_usedTexture = other.m_usedTexture;
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
	m_usedTexture = other.m_usedTexture;
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
	return m_usedTexture;
}

void MgeImage::setTexture(const TextureId& newTextureId)
{
	MAIN_THREAD_GUARD;
	if (newTextureId == m_usedTexture)
		return;

	bool isValidTexture = !m_sprite.expired();
	size_t layer = isValidTexture ? getLayer() : (size_t)GraphicItemLayer::DEFAULT_LAYER;
	int64_t zPosition = isValidTexture ? getZPosition() : mgeCore::getDefaultZPosition();
	DSize currentScale = isValidTexture ? getScale() : DSize(1.0, 1.0);
	MgeColor currentColor = isValidTexture ? getColor() : MgeColor(255, 255, 255, 255);
	currentColor.a = m_isVisible ? m_alpha : 0;

	if (isValidTexture)
		destroySprite();

	m_usedTexture = newTextureId;
	createNewSprite(layer, zPosition, currentScale, currentColor);
}

const std::shared_ptr<MgeLayerObject> MgeImage::getSprite() const noexcept
{
	_ASSERT(!m_sprite.expired());
	return m_sprite.lock();
}

void MgeImage::createNewSprite(size_t usedLayer, int64_t zPosition, const DSize& currentScale, const MgeColor& currentColor)
{
	_ASSERT(m_sprite.expired());
	if (!m_sprite.expired())
		destroySprite();

	_ASSERT(!m_usedTexture.getPath().empty() && m_usedTexture.getPath() != UNDEFINED_TEXTURE_PATH);

	const sf::Texture& usedSfTexture = ML_wrapper::getGlobalMlWrapper()->appendTexture(*this);
	auto newSprite = std::make_shared<MgeLayerObject>(MgeLayerObject{ zPosition, usedLayer, sf::Sprite{usedSfTexture} });
	m_sprite = newSprite;
	_ASSERT(newSprite);
	if (newSprite && std::holds_alternative<sf::Sprite>(newSprite->data))
	{
		auto& sfSprite = std::get<sf::Sprite>(newSprite->data);
		sfSprite.setScale(sf::Vector2f(currentScale.width, currentScale.height));
		sfSprite.setColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, m_isVisible ? currentColor.a : 0));
		sfSprite.setPosition(sf::Vector2f(absolutePosition.x, absolutePosition.y));
	}
	ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newSprite);
	size = mgeType::Size<int>((int)usedSfTexture.getSize().x, (int)usedSfTexture.getSize().y);
}

void MgeImage::destroySprite()
{
	if (auto sprite = m_sprite.lock())
	{
		_ASSERT(m_usedTexture.getPath() != UNDEFINED_TEXTURE_PATH && !m_usedTexture.getPath().empty());
		ML_wrapper::getGlobalMlWrapper()->removeMgeLayerObject(sprite);
		m_sprite.reset();
		_ASSERT(sprite.use_count() == 1); //correct = last instance is local
	}
}

void MgeImage::setColor(const MgeColor& newColor) noexcept
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

MgeColor MgeImage::getColor() const noexcept
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			const auto color = std::get<sf::Sprite>(sprite->data).getColor();
			return MgeColor(color.r, color.g, color.b, color.a);
		}
		else
			_ASSERT(false);
	}
	return MgeColor(255, 255, 255, 255);
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
		return sprite->m_layer;
	return 0;
}

void MgeImage::setLayer(size_t newLayer)
{
	MAIN_THREAD_GUARD;
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
		ML_wrapper::getGlobalMlWrapper()->changeLayerOfMgeObject(sprite, newLayer);
}

void MgeImage::rescale(float scaleX, float scaleY) const
{
	MAIN_THREAD_GUARD;
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			auto& sfSprite = std::get<sf::Sprite>(sprite->data);
			sfSprite.scale(sf::Vector2f(scaleX, scaleY)); //relative - multiplies current scale
		}
		else
			_ASSERT(false);
	}
}

DSize MgeImage::getScale() const
{
	_ASSERT(!m_sprite.expired());
	if (auto sprite = m_sprite.lock())
	{
		if (std::holds_alternative<sf::Sprite>(sprite->data))
		{
			const auto scale = std::get<sf::Sprite>(sprite->data).getScale();
			return DSize(scale.x, scale.y);
		}
		else
		{
			_ASSERT(false);
		}
	}
	return DSize(1.f, 1.f);
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

MgeImage MgeImage::loadTexture(const std::string& path, size_t layer)
{
	return MgeImage(TextureId(path), layer);
}

MgeImage MgeImage::loadTexture(const std::wstring& path, size_t layer)
{
	return MgeImage(TextureId(path), layer);
}

MgeImage MgeImage::loadTexture(const TextureId& textureId, size_t layer)
{
	return MgeImage(TextureId(textureId.getPath()), layer);
}

MgeImage MgeImage::loadImageFromDefaultFolder(const std::string& name, size_t layer, const FPoint& newPosition, std::optional<MgeImage::ImgType> imgFileFormat)
{
	namespace fs = std::filesystem;
	_ASSERT(!m_defaultPath.empty());
	if (m_defaultPath.empty())
		return {};

	fs::path path = m_defaultPath + "\\" + name + generateExtension(imgFileFormat.value_or(MgeImage::ImgType::PNG));
	_ASSERT(fs::exists(m_defaultPath));
	if (!fs::exists(m_defaultPath))
		return {};
	return MgeImage(TextureId(path), layer, newPosition);
}

MgeImage MgeImage::loadImageFromDefaultFolder(const std::wstring& name, size_t layer, const FPoint& newPosition, std::optional<MgeImage::ImgType> imgFileFormat)
{
	return loadImageFromDefaultFolder(mgeCore::toUTF8(name), layer, newPosition, imgFileFormat);
}

void MgeImage::setDefaultImageFolder(const std::wstring& location)
{
	return setDefaultImageFolder(mgeCore::toUTF8(location));
}

void MgeImage::setDefaultImageFolder(const std::string& location)
{
	namespace fs = std::filesystem;
	_ASSERT(fs::exists(location));
	m_defaultPath = location;
}

std::wstring MgeImage::getDefaultImageFolder()
{
	return mgeCore::fromUTF8(m_defaultPath);
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
	destroySprite();
}
