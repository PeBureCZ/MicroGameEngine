#pragma once
#include <unordered_map>
#include <string>
#include <array>
#include <utility>
#include <memory>
#include <optional>
#include "MlImage.h"
#include <map>

#include "SFML\Graphics.hpp"

using TEXTURE_COUNT = size_t;
using TEXTURE_OBJECT = std::pair<TEXTURE_COUNT, sf::Texture>;
using TEXTURES = std::unordered_map<TextureId, TEXTURE_OBJECT>;

using SPRITES = std::unordered_map<SPRITE_ID, sf::Sprite>;
using LAYERED_SPRITES = std::map<size_t, SPRITES>;

class ImageHolder
{
private:
	//ImageID, std::string
	TEXTURES textures;
	LAYERED_SPRITES sprites;

	void removeTexture(TextureId textureId);
	TextureId appendTexture(const MlImage& image);

public:
	ImageHolder();

	void appendImage(const MlImage& image, size_t layer, FPoint position = FPoint());
	void removeImage(TextureId textureId, SPRITE_ID spriteId, size_t layer);

	void appendGuiImage(const MlImage& image, FPoint position = FPoint());
	void removeGuiImage(TextureId textureId, SPRITE_ID spriteId);

	void setImageAbsolutePosition(SPRITE_ID image, size_t layer, FPoint newPosition = FPoint());
	void setSpriteColor(SPRITE_ID image, mgeType::Color_RGBA newColor, size_t layer);
	[[nodiscard]] mgeType::Color_RGBA getSpriteColor(SPRITE_ID image, size_t layer);

	void changeSpriteLayer(const TextureId& textureId, SPRITE_ID imageId, size_t newLayer, size_t oldLayer);

	void setSpriteRotation(SPRITE_ID image, float newRotation, size_t layer);
	[[nodiscard]] float getSpriteRotation(SPRITE_ID image, size_t layer) const;

	[[nodiscard]] mgeType::Size<int> getTextureSize(TextureId id);

	[[nodiscard]] const std::optional<const SPRITES*> getGuiSprites() const;
	[[nodiscard]] const LAYERED_SPRITES& getSprites() const noexcept;

	~ImageHolder();
};

