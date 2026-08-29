#pragma once
#include <unordered_map>
#include <utility>
#include <memory>
#include <string>

#include "SFML\Graphics.hpp"
#include "GraphicDependencies.h"
#include "BasicTypes.h"

using SPRITE_ID = uintptr_t;

struct MgeLayerObject;

class MgeImage
{
public:
	MgeImage(TextureId usedTextureId, size_t usedLayer = (size_t)GraphicItemLayer::DEFAULT_LAYER, FPoint newPosition = FPoint());
	MgeImage();

	MgeImage(const MgeImage& other);
	MgeImage(MgeImage&& other) = default;
	MgeImage& operator= (const MgeImage& other);
	MgeImage& operator= (MgeImage&& other) = default;

	[[nodiscard]] mgeType::Size<int> getSize() const noexcept;

	void setImgAbsolutePosition(FPoint newPosition);
	void setColor(const mgeType::Color_RGBA& newColor) noexcept;
	void setVisible(bool visible);

	void setRotation(float newRotation) const;
	[[nodiscard]] float getRotation() const;
	void setOrigin(const FPoint& newOrigin) const;
	[[nodiscard]] FPoint getAbsolutePosition() const noexcept;
	[[nodiscard]] size_t getLayer() const noexcept;
	void setLayer(size_t newLayer);

	static [[nodiscard]] MgeImage loadTexture(std::string path, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] MgeImage loadTexture(const TextureId& textureId, size_t layer = GraphicItemLayer::DEFAULT_LAYER);

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
	TextureId usedTexture = {};
	FPoint absolutePosition;
	std::weak_ptr<MgeLayerObject>  m_sprite;
	std::uint8_t m_alpha = 255;
	bool m_isVisible = true;
};



