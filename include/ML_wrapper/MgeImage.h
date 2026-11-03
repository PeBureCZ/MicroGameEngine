#pragma once
#include <unordered_map>
#include <utility>
#include <memory>
#include <string>
#include <optional>

#include "SFML\Graphics.hpp"
#include "GraphicDependencies.h"
#include "BasicTypes.h"

using SPRITE_ID = uintptr_t;

struct MgeLayerObject;

class MgeImage
{
public:
	enum class ImgType : int
	{
		Unknown = -1,
		undefined,
		PNG,
		JPG,
		BMP,
		TGA,
		GIF,
		PSD,
		HDR,
		PIC,
		PNM
	};

	MgeImage(const TextureId& usedTextureId, size_t usedLayer = (size_t)GraphicItemLayer::DEFAULT_LAYER, const FPoint& newPosition = FPoint());
	MgeImage();

	MgeImage(const MgeImage& other);
	MgeImage(MgeImage&& other) = default;
	MgeImage& operator= (const MgeImage& other);
	MgeImage& operator= (MgeImage&& other) = default;

	[[nodiscard]] mgeType::Size<int> getSize() const noexcept;

	void setImgAbsolutePosition(FPoint newPosition);
	void setColor(const MgeColor& newColor) noexcept;
	MgeColor getColor() const noexcept;
	void setVisible(bool visible);

	void setRotation(float newRotation) const;
	[[nodiscard]] float getRotation() const;
	void setOrigin(const FPoint& newOrigin) const;
	[[nodiscard]] FPoint getAbsolutePosition() const noexcept;
	[[nodiscard]] size_t getLayer() const noexcept;
	void setLayer(size_t newLayer);

	void rescale(float scaleX, float scaleY) const;
	[[nodiscard]] DSize getScale() const;
	[[nodiscard]] bool isValid() const noexcept;

	static [[nodiscard]] MgeImage loadTexture(const std::string& path, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] MgeImage loadTexture(const std::wstring& path, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] MgeImage loadTexture(const TextureId& textureId, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] MgeImage loadImageFromDefaultFolder(const std::string& name, size_t layer = GraphicItemLayer::DEFAULT_LAYER,
		const FPoint& newPosition = FPoint(), std::optional<ImgType> imgFileFormat = std::nullopt);
	static [[nodiscard]] MgeImage loadImageFromDefaultFolder(const std::wstring& name, size_t layer = GraphicItemLayer::DEFAULT_LAYER,
		const FPoint& newPosition = FPoint(), std::optional<ImgType> imgFileFormat = std::nullopt);
	static void setDefaultImageFolder(const std::wstring& location);
	static void setDefaultImageFolder(const std::string& location);
	static [[nodiscard]] std::wstring getDefaultImageFolder();

	void setZPosition(int64_t newZPosition) noexcept;
	[[nodiscard]] int64_t getZPosition() const noexcept;

	~MgeImage();

	[[nodiscard]] SPRITE_ID getSpriteID() const noexcept;
	[[nodiscard]] TextureId getTextureID() const noexcept;
	void setTexture(const TextureId& newTextureId);
	[[nodiscard]] const std::shared_ptr<MgeLayerObject> getSprite() const noexcept;

private:

	void createNewSprite(size_t usedLayer, int64_t zPosition = mgeCore::getDefaultZPosition(), const DSize& currentScale = DSize(1.0, 1.0),
		const MgeColor& currentColor = MgeColor(255, 255, 255, 255));
	void destroySprite();

	mgeType::Size<int> size;
	SPRITE_ID imageId = (SPRITE_ID)0;

	//todo: it will be reworked to allow the use of multiple textures stored in the cache
	TextureId m_usedTexture;

	FPoint absolutePosition;
	std::weak_ptr<MgeLayerObject>  m_sprite;
	std::uint8_t m_alpha = 255;
	bool m_isVisible = true;
};



