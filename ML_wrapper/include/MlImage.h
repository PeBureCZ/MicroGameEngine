#pragma once
#include <unordered_map>
#include <utility>
#include <memory>
#include <string>

#include "SFML\Graphics.hpp"
#include "GraphicDependencies.h"
#include "BasicTypes.h"


using SPRITE_ID = uintptr_t;

class MlImage;
using uPtr_MlImage = std::unique_ptr<MlImage>;

class MlImage
{
private:
	tsmType::Size<int> size;
	SPRITE_ID imageId = 0;
	TextureId usedTexture = {};
	FPoint absolutePosition;
	size_t layer = GraphicItemLayer::DEFAULT_LAYER;
public:
	MlImage(TextureId usedTextureId, size_t usedLayer = (size_t)GraphicItemLayer::DEFAULT_LAYER, FPoint newPosition = FPoint());
	MlImage();

	MlImage(const MlImage& other);
	MlImage(const MlImage&& other) noexcept;
	MlImage& operator= (const MlImage& other);
	MlImage& operator= (const MlImage&& other) noexcept;

	[[nodiscard]] tsmType::Size<int> getSize() const noexcept;

	void setImgAbsolutePosition(FPoint newPosition);
	void setColor(tsmType::Color_RGBA& newColor) const noexcept;
	void setVisible(bool visible) const noexcept;

	void setRotation(float newRotation) const noexcept;
	void setOrigin(FPoint newOrigin) const noexcept;
	[[nodiscard]] float getRotation() const noexcept;
	[[nodiscard]] FPoint getAbsolutePosition() const noexcept;
	size_t getLayer() const noexcept;
	void setLayer(size_t newLayer) noexcept;

	static [[nodiscard]] uPtr_MlImage loadTexture(std::string path, size_t layer = GraphicItemLayer::DEFAULT_LAYER);
	static [[nodiscard]] uPtr_MlImage loadTexture(const TextureId& textureId, size_t layer = GraphicItemLayer::DEFAULT_LAYER);

	~MlImage();

	SPRITE_ID getSpriteID() const noexcept;
	TextureId getTextureID() const noexcept;
};



