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
	MGE_GRAPHIC_PTR getVariant(size_t index);

	size_t getComponentSize();
	void setIsVisible(bool visible);
	std::optional<size_t> getLayerFromVariant(size_t index);
	bool setLayerToVariant(size_t index, GraphicItemLayer newLayer);

	void setColor(const MgeColor& newColor, size_t index);

	void rescaleGraphic(float scaleX, float scaleY, size_t index);
	void setRotation(float rotation, size_t index);
	void setPosition(const FPoint& position, size_t index);
	[[nodiscard]] std::optional<FPoint> getPosition(size_t index);


	/** * @brief Sets the Z-position of a graphic component within its layer.
	@param newZPosition The new Z-position determining the drawing order within the layer.
	@param index The index of the graphic component in m_graphic. */
	void setZPosition(const int64_t newZPosition, size_t index) noexcept;

	/** * @brief Returns the Z-position of a graphic component within its layer.
	@param index The index of the graphic component in m_graphic.
	@return The Z-position, or std::nullopt if the component does not exist */
	std::optional<int64_t> getZPosition(size_t index) noexcept;

	/**@brief Sets the draw group (primary sort key inside the layer) to all graphic objects of the component. */
	void setDrawGroup(const int64_t newDrawGroup) noexcept;

	void setOrigin(const FPoint& newOrigin, size_t index);
	[[nodiscard]] std::optional<float> getRotation(size_t index);

private:

	std::vector<MGE_GRAPHIC_PTR> m_graphic;
};