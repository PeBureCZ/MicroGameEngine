#pragma once

#include "MgeComponents.h"

#include <memory>
#include <variant>
#include <vector>
#include <string_view>
#include <optional>

#include "MgeImage.h"
#include "MgeDrawable.h"
#include "MgeText.h"

using UNDEFINED_GRAPHIC_OBJECT = bool;
using MGE_GRAPHIC_VARIANT = std::variant<UNDEFINED_GRAPHIC_OBJECT, MgeImage, MgeDrawable, MgeText>;

class MgeGraphicComponent;
using MGE_GRAPHIC = std::shared_ptr<MgeGraphicComponent>;

class MgeGraphicComponent : public MgeBasicComponent
{
public:
	MgeGraphicComponent() = default;
	MgeGraphicComponent(MGE_GRAPHIC_VARIANT&& graphicObject);
	MgeGraphicComponent(std::string_view text, unsigned int characterSize_pxls = 30);

	void addGraphicVariant(MGE_GRAPHIC_VARIANT&& graphicObject);
	void setVariant(MGE_GRAPHIC_VARIANT&& graphicObject, size_t index);

	size_t getComponentSize();
	void setIsVisible(bool visible);
	std::optional<size_t> getLayerFromVariant(size_t index);

	void setColor(const mgeType::Color_RGBA& newColor, size_t index);

	void setRotation(float rotation, size_t index);
	void setPosition(const FPoint& position, size_t index);
	[[nodiscard]] std::optional<FPoint> getPosition(size_t index);

	void setOrigin(const FPoint& newOrigin, size_t index);
	[[nodiscard]] std::optional<float> getRotation(size_t index);

private:

	std::vector<MGE_GRAPHIC_VARIANT> m_graphic;
};