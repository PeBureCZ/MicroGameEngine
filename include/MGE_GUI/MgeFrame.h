#pragma once
#include <variant>
#include <vector>
#include <deque>
#include <memory>

#include "MgeWidget.h"
#include "MgeImage.h"
#include "MgeText.h"
#include "MgeDrawable.h"
#include "GraphicDependencies.h"
#include "MgeGraphicComponent.h"

constexpr mgeType::Color_RGBA DEFAULT_TEXT_COLOR = mgeType::Color_RGBA(0, 0, 0, 255);
constexpr mgeType::Color_RGBA MOUSE_OVER_TEXT_COLOR = mgeType::Color_RGBA(0, 0, 0, 255);
constexpr mgeType::Color_RGBA DEFAULT_FRAME_COLOR = mgeType::Color_RGBA(200, 200, 200, 255);

class MgeButton;
class MgeFrame;

using UNDEFINED_FRAME_OBJECT = bool;
using FRAME_OBJECT = std::variant<UNDEFINED_FRAME_OBJECT, MgeImage, MgeDrawable>;

enum GraphicType : size_t
{
	BASIC_GRAPHIC_INDEX = 0,
};

enum class BorderSide : uint8_t
{
	None = 0,
	Top = 1 << 0,
	Right = 1 << 1,
	Bottom = 1 << 2,
	Left = 1 << 3,

	All = Top | Right | Bottom | Left
};

constexpr BorderSide operator|(BorderSide lhs, BorderSide rhs)
{
	return static_cast<BorderSide>(
		static_cast<uint8_t>(lhs) |
		static_cast<uint8_t>(rhs));
}

namespace mge
{
	using Frame = std::shared_ptr<MgeFrame>;
	Frame createFrame(const FPoint& newPosition = FPoint(), const ISize& newSize = ISize(1, 1),
		GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER, mgeType::Color_RGBA color = DEFAULT_FRAME_COLOR);
	Frame createFrame(const FPoint& newPosition, const TextureId& textureId, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
}

class MgeFrame : public MgeWidget
{
public:
	MgeFrame(const FPoint& newPosition, const ISize& newSize, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER,
		mgeType::Color_RGBA color = DEFAULT_FRAME_COLOR);
	MgeFrame(const FPoint& newPosition, const TextureId& textureId, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);

	void setImage(TextureId textureId);
	void setImage(MgeImage&& image);
	void setVertices(MgeDrawable&& newVertices) noexcept;

	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	void setColor(const mgeType::Color_RGBA& newColor);

	void setIsVisible(bool visible) noexcept override;

	void setRelativeRotation(float newRotation);
	float getRelativeRotation(); 
	void setOrigin(IPoint newOrigin);
	
	void setBorder(BorderSide sides, unsigned int width_pxls, const mgeType::Color_RGBA& color = mgeType::Color_RGBA());

	//function is called automatically from GUI
	void setUnderMouseCursor(bool isUnderMouse);

	MGE_GRAPHIC getGraphicComponent();

	void layout() noexcept override;

	[[nodiscard]] bool isUnderCursor() const noexcept;

protected:

	IPoint getAlignedPosition(GuiAlign align, mgeType::Size<int> objectSize);

private:

	MgeDrawable borderObject;

	BorderSide m_borderFlags = BorderSide::None;
	unsigned int m_borderWidth_pxls = 0;
	mgeType::Color_RGBA m_borderColor;

	void setGraphicObject(const MGE_GRAPHIC_PTR& object);
};


