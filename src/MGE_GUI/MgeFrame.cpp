#include "MgeFrame.h"

#include "BasicShapes.h"
#include "BasicTypes.h"
#include "MgeDrawable.h"
#include "MlWrapper.h"
#include "Trigger.h"
#include "MgeText.h"



MgeFrame::MgeFrame(const FPoint& newPosition, const ISize& newSize, GraphicItemLayer layer, mgeType::Color_RGBA color)
	: MgeWidget(newPosition, newSize)
{
	const MgeVertices<float> drawable(mgeShape::Rectangle<float>(FPoint(0,0), FSize((float)newSize.width, (float)newSize.height)), color);
	MgeDrawable newWidgetVertices(drawable, newPosition.asFloat(), 0.f, layer);
	auto graphic = std::make_shared<MgeGraphicComponent>(std::move(newWidgetVertices));
	addComponent(std::move(graphic));
}

MgeFrame::MgeFrame(const FPoint& newPosition, const TextureId& textureId, GraphicItemLayer layer)
	: MgeWidget(newPosition, ISize(1,1))
{
	auto newImage = MgeImage(textureId, layer, newPosition.asFloat());;
	setSize(newImage.getSize());
	std::shared_ptr<MgeGraphicComponent> graphic = std::make_shared<MgeGraphicComponent>(std::move(newImage));
	addComponent(std::move(graphic));
}

void MgeFrame::setImage(TextureId textureId)
{
	auto image = MgeImage(std::move(textureId), GraphicItemLayer::GUI_LAYER, getAbsolutePosition().asFloat());
	setSize(image.getSize());
	auto newImageVariant = std::make_shared<MGE_GRAPHIC_VARIANT>(std::move(image));
	setGraphicObject(newImageVariant);
	editCollision().clear();
	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition().asInt(), getSize())));
}

void MgeFrame::setImage(MgeImage&& image)
{
	_ASSERT(image.getLayer() == GraphicItemLayer::GUI_LAYER
		|| image.getLayer() == GraphicItemLayer::WINDOW_LAYER); //image should be in GUI or WIN layer to avoid unrelated issues

	setSize(image.getSize());
	auto newImageVariant = std::make_shared<MGE_GRAPHIC_VARIANT>(std::move(image));
	setGraphicObject(newImageVariant);
	editCollision().clear();
	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition().asInt(), getSize())));
}

void MgeFrame::setVertices(MgeDrawable&& newVertices) noexcept
{
	auto newGraphicObj = std::make_shared<MGE_GRAPHIC_VARIANT>(std::move(newVertices));
	setGraphicObject(newGraphicObj);
}

std::vector<Trigger<int>>& MgeFrame::editCollision() noexcept
{
	return collisions;
}

const std::vector<Trigger<int>>& MgeFrame::getCollision() const noexcept
{
	return collisions;
}

void MgeFrame::addCollision(Trigger<int> addedCollision)
{
	auto difPos_pxl = (getParent()) ? getRelativePosition() : getAbsolutePosition();
	addedCollision.setAbsolutePosition(addedCollision.getAbsolutePosition() + difPos_pxl.asInt());
	collisions.push_back(std::move(addedCollision));
}

void MgeFrame::setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	setColor(mgeType::Color_RGBA(r,g,b,a));
}

void MgeFrame::setColor(const mgeType::Color_RGBA& newColor)
{
	if (auto graphicComponent = getGraphicComponent())
		graphicComponent->setColor(newColor, BASIC_GRAPHIC_INDEX);
	else
	{
		_ASSERT(false); //should not happened!
	}
}

void MgeFrame::setIsVisible(bool visible) noexcept
{
	MgeWidget::setIsVisible(visible);

	if (auto graphic = getGraphicComponent())
		graphic->setIsVisible(visible);

	for (auto& child : editChildren())
	{
		if (auto widget = std::dynamic_pointer_cast<MgeWidget>(child))
			widget->setIsVisible(visible);
	}
}

void MgeFrame::setRelativeRotation(float newRotation)
{
	auto difRotation = newRotation - getRelativeRotation();
	if (auto graphic = getGraphicComponent())
		graphic->setRotation(newRotation, BASIC_GRAPHIC_INDEX);

	for (auto& col : collisions)
		col.setRotation(col.getRotation() + difRotation);
}

float MgeFrame::getRelativeRotation()
{
	if (auto graphic = getGraphicComponent())
		return graphic->getRotation(BASIC_GRAPHIC_INDEX).value_or(0.f);
	return {0.f};
}

void MgeFrame::setOrigin(IPoint newOrigin)
{
	if (auto graphic = getGraphicComponent())
		graphic->setOrigin(newOrigin.asFloat(), BASIC_GRAPHIC_INDEX);
}

void MgeFrame::setBorder(BorderSide sides, unsigned int width_pxls, const mgeType::Color_RGBA& color)
{
	auto flags = static_cast<uint8_t>(sides);
	m_borderFlags = sides;
	m_borderWidth_pxls = width_pxls;
	m_borderColor = color;

	if (flags & static_cast<uint8_t>(BorderSide::None))
	{
		borderObject = false;
		return;
	}

	std::vector<MgeVertices<float>> vecVertices;
	ISize actualSize = getSize();

	if (flags & static_cast<uint8_t>(BorderSide::Top))
	{
		auto shape = mgeShape::Rectangle<float>(FPoint(0.f, 0.f), FSize((float)actualSize.width, (float)m_borderWidth_pxls));
		vecVertices.emplace_back(shape, m_borderColor);
	}

	if (flags & static_cast<uint8_t>(BorderSide::Right))
	{
		auto shape = mgeShape::Rectangle<float>(FPoint((float)actualSize.width - (float)m_borderWidth_pxls, (float)m_borderWidth_pxls), FSize((float)m_borderWidth_pxls, (float)actualSize.height - (float)m_borderWidth_pxls));
		vecVertices.emplace_back(shape, m_borderColor);
	}

	if (flags & static_cast<uint8_t>(BorderSide::Bottom))
	{
		auto shape = mgeShape::Rectangle<float>(FPoint(0.f, (float)actualSize.height - (float)m_borderWidth_pxls), FSize((float)actualSize.width, (float)m_borderWidth_pxls));
		vecVertices.emplace_back(shape, m_borderColor);
	}

	if (flags & static_cast<uint8_t>(BorderSide::Left))
	{
		auto shape = mgeShape::Rectangle<float>(FPoint(0.f, (float)m_borderWidth_pxls), FSize((float)m_borderWidth_pxls, (float)actualSize.height - (float)m_borderWidth_pxls));
		vecVertices.emplace_back(shape, m_borderColor);
	}

	size_t layer = GraphicItemLayer::GUI_LAYER;
	if (auto graphic = getGraphicComponent())
		layer = graphic->getLayerFromVariant(BASIC_GRAPHIC_INDEX).value_or(GraphicItemLayer::GUI_LAYER);

	MgeDrawable borders(layer);
	borders.addObjects(std::move(vecVertices));
	borders.setPosition(getAbsolutePosition());
	borderObject = std::move(borders);
}

void MgeFrame::setOnCursorOver(Callback_deprecated cursorEnterFunction, Callback_deprecated cursorLeaveFunction)
{
	onCursorEnter = std::move(cursorEnterFunction);
	onCursorLeave = std::move(cursorLeaveFunction);
}

void MgeFrame::layout() noexcept
{
#ifdef _DEBUG
	[[maybe_unused]] auto id = getId();
	if (id > 0)
		id = id;
#endif
	try
	{
		auto differencePos = getAbsolutePosition() - lastLayoutAbsolutePosition;

		const auto sizeChanged = (lastLayoutSize != getSize());
		_ASSERT(lastLayoutSize.width > 0.0001 && lastLayoutSize.height > 0.0001);
		float scaleX = (lastLayoutSize.width > 0.0001)
			? (float)getSize().width / (float)lastLayoutSize.width
			: 1.f;
		float scaleY = ((float)lastLayoutSize.height > 0.0001)
			? (float)getSize().height / (float)lastLayoutSize.height
			: 1.f;

		if (editCollision().size() > 0)
		{
			for (auto& col : editCollision())
			{
				col.setAbsolutePosition(col.getAbsolutePosition() + differencePos.asInt());

				if (sizeChanged && col.getType() == SHAPE_TYPE::box)
					col.rescaleShape(scaleX, scaleY);
			}
		}

		setBorder(m_borderFlags, m_borderWidth_pxls, m_borderColor); //re-draw border with aktual size

		//FRAME GRAPHIC OBJECT
		if (auto graphic = getGraphicComponent())
		{
			if (sizeChanged)
				graphic->rescaleGraphic(scaleX, scaleY, BASIC_GRAPHIC_INDEX);

			auto difPos = graphic->getPosition(BASIC_GRAPHIC_INDEX).value_or(FPoint()) + differencePos.asFloat();
			graphic->setPosition(difPos, BASIC_GRAPHIC_INDEX);
		}

		MgeWidget::layout();
	} //try block end
#ifdef _DEBUG
	catch ([[maybe_unused]] std::exception& e)
	{
		_ASSERT(false);
	}
#endif
	catch (...)
	{
		_ASSERT(false);
	}
}

void MgeFrame::onCursorEnterCall() noexcept
{
	if (onCursorEnter)
		onCursorEnter();
}

void MgeFrame::onCursorLeaveCall() noexcept
{
	if (onCursorLeave)
		onCursorLeave();
}

void MgeFrame::setUnderMouseCursor(bool isUnderMouse)
{
	if (!isUnderMouseCursor && isUnderMouse)
		onCursorEnterCall();
	else if (isUnderMouseCursor && !isUnderMouse)
		onCursorLeaveCall();
	isUnderMouseCursor = isUnderMouse;
}

MGE_GRAPHIC MgeFrame::getGraphicComponent()
{
	if (auto graphicComponent = std::dynamic_pointer_cast<MgeGraphicComponent>(editComponent(MgePredefinedComponents::GRAPHIC)))
		return graphicComponent;
	return nullptr;
}

bool MgeFrame::isUnderCursor() const noexcept
{
	return isUnderMouseCursor;
}

IPoint MgeFrame::getAlignedPosition(GuiAlign align, mgeType::Size<int> objectSize)
{
	IPoint framePos = getAbsolutePosition().asInt();
	auto frameSize = getSize();

	switch (align)
	{
		case GuiAlign::TopLeft:
			return framePos;
		case GuiAlign::TopCenter:
			return IPoint(framePos.x + frameSize.width / 2 - objectSize.width / 2, framePos.y);
		case GuiAlign::TopRight:
			return IPoint(framePos.x + frameSize.width - objectSize.width, framePos.y);
		case GuiAlign::MiddleLeft:
			return IPoint(framePos.x, framePos.y + frameSize.height / 2 - objectSize.height / 2);
		case GuiAlign::MiddleCenter:
			return IPoint(framePos.x + frameSize.width / 2 - objectSize.width / 2,
				framePos.y + frameSize.height / 2 - objectSize.height / 2);
		case GuiAlign::MiddleRight:
			return IPoint(framePos.x + frameSize.width - objectSize.width,
				framePos.y + frameSize.height / 2 - objectSize.height / 2);
		case GuiAlign::BottomLeft:
			return IPoint(framePos.x, framePos.y + frameSize.height - objectSize.height);
		case GuiAlign::BottomCenter:
			return IPoint(framePos.x + frameSize.width / 2 - objectSize.width / 2,
				framePos.y + frameSize.height - objectSize.height);
		case GuiAlign::BottomRight:
			return IPoint(framePos.x + frameSize.width - objectSize.width,
				framePos.y + frameSize.height - objectSize.height);
		default:
			_ASSERT(false); //unknown alignment
			break;
	}
	return framePos;
}

void MgeFrame::setGraphicObject(const MGE_GRAPHIC_PTR& object)
{
	if (auto graphicComponent = getGraphicComponent())
		graphicComponent->setVariant(object, BASIC_GRAPHIC_INDEX);
	else
		{ _ASSERT(false); }
}

namespace mge
{
	Frame createFrame(const FPoint& newPosition, const ISize& newSize, GraphicItemLayer layer, mgeType::Color_RGBA color)
	{
		auto newFrame = std::make_shared<MgeFrame>(newPosition, newSize, layer, color);
		newFrame->initializeSelf(newFrame);
		return newFrame;
	}

	Frame mge::createFrame(const FPoint& newPosition, const TextureId& textureId, GraphicItemLayer layer)
	{
		auto newFrame = std::make_shared<MgeFrame>(newPosition, textureId, layer);
		newFrame->initializeSelf(newFrame);
		return newFrame;
	}
}
