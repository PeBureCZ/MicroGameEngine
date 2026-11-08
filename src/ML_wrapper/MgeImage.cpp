#include "MgeImage.h"

#include "MlWrapper.h"
#include "GlobalFunctions.h"
#include <filesystem>

namespace
{
	std::wstring_view generateExtension(MgeImage::ImgType type)
	{
		switch (type)
		{
		case MgeImage::ImgType::BMP: return L".bmp";
		case MgeImage::ImgType::PNG: return L".png";
		case MgeImage::ImgType::JPG: return L".jpg";
		case MgeImage::ImgType::TGA: return L".tga";
		case MgeImage::ImgType::GIF: return L".gif";
		case MgeImage::ImgType::PSD: return L".psd";
		case MgeImage::ImgType::HDR: return L".hdr";
		case MgeImage::ImgType::PIC: return L".pic";
		case MgeImage::ImgType::PNM: return L".pnm";
		default: _ASSERT(false); //undefined or unknown type
		}
		return L".png";
	}

	std::filesystem::path g_defaultImageFolder;
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
	copySpriteFrom(other);
}

MgeImage::MgeImage(MgeImage&& other) noexcept
{
	imageId = reinterpret_cast<VerticesId>(this); //own address, not other's
	takeSpriteFrom(other);
}

MgeImage& MgeImage::operator=(const MgeImage& other)
{
	MAIN_THREAD_GUARD;
	if (this == &other)
		return *this;

	destroySprite(); //otherwise the old sprite stays in its layer: drawn forever, never freed
	copySpriteFrom(other);
	return *this;
}

MgeImage& MgeImage::operator=(MgeImage&& other) noexcept
{
	MAIN_THREAD_GUARD;
	if (this == &other)
		return *this;

	destroySprite(); //otherwise the old sprite stays in its layer: drawn forever, never freed
	takeSpriteFrom(other);
	return *this;
}

void MgeImage::copySpriteFrom(const MgeImage& other)
{
	MAIN_THREAD_GUARD;
	if (!m_sprite.expired())
		destroySprite();

	size = other.size;
	m_usedTexture = other.m_usedTexture;
	absolutePosition = other.absolutePosition;
	m_alpha = other.m_alpha;
	m_isVisible = other.m_isVisible;

	const auto otherSprite = other.m_sprite.lock();
	if (!otherSprite || !std::holds_alternative<sf::Sprite>(otherSprite->data))
	{
		_ASSERT(false); //wrong sprite to copy
		return;
	}

	auto mlWrapper = ML_wrapper::getGlobalMlWrapper();
	mlWrapper->appendTexture(*this); //texture reference count only: the copied sf::Sprite already points to the same texture

	//copy of the whole layer object = texture, layer, z-position, position, scale, origin, rotation, color (incl. hidden alpha)
	auto newSprite = std::make_shared<MgeLayerObject>(*otherSprite);

	m_sprite = newSprite;
	mlWrapper->addMgeLayerObject(newSprite);
}

void MgeImage::takeSpriteFrom(MgeImage& other) noexcept
{
	_ASSERT(m_sprite.expired());

	size = other.size;
	m_usedTexture = std::move(other.m_usedTexture);
	absolutePosition = other.absolutePosition;
	m_alpha = other.m_alpha;
	m_isVisible = other.m_isVisible;
	m_sprite = std::move(other.m_sprite);
	other.m_sprite.reset(); //other must not remove the sprite in its destructor
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

	_ASSERT(!m_usedTexture.getPath().empty());

	const sf::Texture& usedSfTexture = ML_wrapper::getGlobalMlWrapper()->appendTexture(*this);
	auto newSprite = std::make_shared<MgeLayerObject>(MgeLayerObject{ zPosition, usedLayer, sf::Sprite{usedSfTexture} });
	m_sprite = newSprite;
	_ASSERT(newSprite);
	if (newSprite && std::holds_alternative<sf::Sprite>(newSprite->data))
	{
		auto& sfSprite = std::get<sf::Sprite>(newSprite->data);
		sfSprite.setScale(sf::Vector2f((float)currentScale.width, (float)currentScale.height));
		sfSprite.setColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, m_isVisible ? currentColor.a : 0));
		sfSprite.setPosition(sf::Vector2f(absolutePosition.x, absolutePosition.y));
	}
	ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newSprite);
	size = mgeType::Size<int>((int)usedSfTexture.getSize().x, (int)usedSfTexture.getSize().y);
}

void MgeImage::destroySprite() noexcept
{
	try
	{
		if (auto sprite = m_sprite.lock())
		{
			_ASSERT(!m_usedTexture.getPath().empty());
			ML_wrapper::getGlobalMlWrapper()->removeMgeLayerObject(sprite);
			m_sprite.reset();
			_ASSERT(sprite.use_count() == 1); //correct = last instance is local
		}
	}
	catch (...)
	{
		_ASSERT(false); //called from the destructor and the move assignment, must not throw
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

bool MgeImage::isValid() const noexcept
{
	return !m_sprite.expired() && !m_usedTexture.getPath().empty();
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
	_ASSERT(!g_defaultImageFolder.empty());
	if (g_defaultImageFolder.empty())
		return {};

	std::error_code errorCode;
	const bool folderExists = fs::exists(g_defaultImageFolder, errorCode);
	_ASSERT(folderExists);
	if (!folderExists)
		return {};

	fs::path path = g_defaultImageFolder / std::filesystem::path(mgeCore::fromUTF8(name));
	path += generateExtension(imgFileFormat.value_or(MgeImage::ImgType::PNG));
	return MgeImage(TextureId(path), layer, newPosition);
}

MgeImage MgeImage::loadImageFromDefaultFolder(const std::wstring& name, size_t layer, const FPoint& newPosition, std::optional<MgeImage::ImgType> imgFileFormat)
{
	return loadImageFromDefaultFolder(mgeCore::toUTF8(name), layer, newPosition, imgFileFormat);
}

void MgeImage::setDefaultImageFolder(const std::filesystem::path& location)
{
	std::error_code errorCode;
	_ASSERT(std::filesystem::exists(location, errorCode));
	g_defaultImageFolder = location;
}

void MgeImage::setDefaultImageFolder(const std::string& location_utf8)
{
	setDefaultImageFolder(std::filesystem::path(mgeCore::fromUTF8(location_utf8)));
}

std::wstring MgeImage::getDefaultImageFolder()
{
	return g_defaultImageFolder.native();
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
