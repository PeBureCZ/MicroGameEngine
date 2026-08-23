#pragma once
#include <unordered_map>
#include <utility>
#include <memory>
#include <string>

#include "SFML\Graphics.hpp"
#include "GraphicDependencies.h"
#include "BasicTypes.h"

using SPRITE_ID = uintptr_t;

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
	size_t getLayer() const noexcept;
	void setLayer(size_t newLayer);

	static [[nodiscard]] MgeImage loadTexture(std::string path, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] MgeImage loadTexture(const TextureId& textureId, size_t layer = GraphicItemLayer::DEFAULT_LAYER);

	~MgeImage();

	SPRITE_ID getSpriteID() const noexcept;
	TextureId getTextureID() const noexcept;
	const std::shared_ptr<sf::Sprite> getSprite() const noexcept;
protected:

private:
	mgeType::Size<int> size;
	SPRITE_ID imageId = (SPRITE_ID)0;
	TextureId usedTexture = {};
	FPoint absolutePosition;
	size_t layer = GraphicItemLayer::DEFAULT_LAYER;
	std::shared_ptr<sf::Sprite> sprite;
	std::uint8_t m_alpha = 255;
	bool m_isVisible = true;
};



