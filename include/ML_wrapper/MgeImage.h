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
	void setVisible(bool visible);

	void setRotation(float newRotation) const;
	[[nodiscard]] float getRotation() const;
	void setOrigin(const FPoint& newOrigin) const;
	[[nodiscard]] FPoint getAbsolutePosition() const noexcept;
	[[nodiscard]] size_t getLayer() const noexcept;
	void setLayer(size_t newLayer);

	static [[nodiscard]] MgeImage loadTexture(const std::string& path, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] MgeImage loadTexture(const std::wstring& path, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] MgeImage loadTexture(const TextureId& textureId, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] MgeImage loadImageFromDefaultFolder(const std::string& name, size_t layer = GraphicItemLayer::DEFAULT_LAYER,
		const FPoint& newPosition = FPoint(), std::optional<ImgType> imgFileFormat = std::nullopt);
	static [[nodiscard]] MgeImage loadImageFromDefaultFolder(const std::wstring& name, size_t layer = GraphicItemLayer::DEFAULT_LAYER,
		const FPoint& newPosition = FPoint(), std::optional<ImgType> imgFileFormat = std::nullopt);
	static [[nodiscard]] void setDefaultImageFolder(const std::wstring& location);
	static [[nodiscard]] void setDefaultImageFolder(const std::string& location);

	void setZPosition(int64_t newZPosition) noexcept;
	[[nodiscard]] int64_t getZPosition() const noexcept;

	~MgeImage();

	[[nodiscard]] SPRITE_ID getSpriteID() const noexcept;
	[[nodiscard]] TextureId getTextureID() const noexcept;
	[[nodiscard]] const std::shared_ptr<MgeLayerObject> getSprite() const noexcept;

protected:

private:
	mgeType::Size<int> size;
	SPRITE_ID imageId = (SPRITE_ID)0;
	TextureId usedTexture;
	FPoint absolutePosition;
	std::weak_ptr<MgeLayerObject>  m_sprite;
	std::uint8_t m_alpha = 255;
	bool m_isVisible = true;
};



