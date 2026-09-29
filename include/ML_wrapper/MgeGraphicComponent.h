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
using MGE_GRAPHIC_PTR = std::shared_ptr<MGE_GRAPHIC_VARIANT>;

class MgeGraphicComponent;
using MGE_GRAPHIC = std::shared_ptr<MgeGraphicComponent>;

// use this class when you need to store multiple graphic objects in one component or when the object can be drawn in different ways
// (for example, object can be image and sometimes change its state to drawable, etc.)
class MgeGraphicComponent : public MgeBasicComponent
{
public:
	MgeGraphicComponent() = default;
	MgeGraphicComponent(const MGE_GRAPHIC_PTR& graphicObject);
	MgeGraphicComponent(MgeImage&& graphicObject);
	MgeGraphicComponent(MgeDrawable&& graphicObject);
	MgeGraphicComponent(MgeText&& graphicObject);
	MgeGraphicComponent(std::string_view text, unsigned int characterSize_pxls = 30);

	MgeGraphicComponent(MgeGraphicComponent&) = delete;
	MgeGraphicComponent(MgeGraphicComponent&& ) = delete;
	MgeGraphicComponent operator=(MgeGraphicComponent&) = delete;
	MgeGraphicComponent operator=(MgeGraphicComponent&&) = delete;

	~MgeGraphicComponent();

	void addGraphicVariant(const MGE_GRAPHIC_PTR& graphicObject);
	void setVariant(const MGE_GRAPHIC_PTR& graphicObject, size_t index);

	size_t getComponentSize();
	void setIsVisible(bool visible);
	std::optional<size_t> getLayerFromVariant(size_t index);

	void setColor(const mgeType::Color_RGBA& newColor, size_t index);

	void rescaleGraphic(float scaleX, float scaleY, size_t index);
	void setRotation(float rotation, size_t index);
	void setPosition(const FPoint& position, size_t index);
	[[nodiscard]] std::optional<FPoint> getPosition(size_t index);

	void setOrigin(const FPoint& newOrigin, size_t index);
	[[nodiscard]] std::optional<float> getRotation(size_t index);

private:

	std::vector<MGE_GRAPHIC_PTR> m_graphic;
};