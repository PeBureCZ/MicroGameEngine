#include "MgeFrame.h"

#include "BasicShapes.h"
#include "BasicTypes.h"
#include "MgeDrawable.h"
#include "MlWrapper.h"
#include "GraphicDependencies.h"
#include "Trigger.h"
#include "MgeText.h"


MgeFrame::MgeFrame(const IPoint& newPosition, const ISize& newSize)
	: MgeWidget(newPosition, newSize)
{
	const MgeVertices<float> drawable(mgeShape::Rectangle<float>(FPoint(0,0), mgeType::Size<float>((float)newSize.width, (float)newSize.height)), mgeType::Color_RGBA(100, 100, 100, 150));
	MgeDrawable newWidgetVertices(drawable, newPosition.asFloat(), 0.f, GraphicItemLayer::GUI_LAYER);
	frameObject = std::move(newWidgetVertices);
}

MgeFrame::MgeFrame(const IPoint& newPosition, TextureId textureId)
	: MgeWidget(newPosition, mgeType::Size<int>())
{
	auto newImage = MgeImage(std::move(textureId), GraphicItemLayer::GUI_LAYER, newPosition.asFloat());;
	setSize(newImage.getSize());
	frameObject = std::move(newImage);
}

void MgeFrame::setImage(TextureId textureId)
{
	auto image = MgeImage(std::move(textureId), GraphicItemLayer::GUI_LAYER, getAbsolutePosition().asFloat());
	setSize(image.getSize());
	frameObject = std::move(image);
	editCollision().clear();
	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition(), getSize())));
}

void MgeFrame::setVertices(MgeDrawable&& newVertices) noexcept
{
	frameObject = std::move(newVertices);
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
	addedCollision.setAbsolutePosition(addedCollision.getAbsolutePosition() + difPos_pxl);
	collisions.push_back(std::move(addedCollision));
}

void MgeFrame::setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	setColor(mgeType::Color_RGBA(r,g,b,a));
}

void MgeFrame::setColor(const mgeType::Color_RGBA& newColor)
{
	if (std::holds_alternative<MgeDrawable>(frameObject))
	{
		auto& drawableObject = std::get<MgeDrawable>(frameObject);
		drawableObject.setColor(newColor);
	}
	else if (std::holds_alternative<MgeImage>(frameObject))
	{
		auto& img = std::get<MgeImage>(frameObject);
		img.setColor(newColor);
	}
	else
		{ _ASSERT(false); } //unhandled
}

const FRAME_OBJECT& MgeFrame::getFrameObject() const noexcept
{
	return frameObject;
}

void MgeFrame::addTextToFrame(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align, const mgeType::Color_RGBA& col)
{
	MgeText newText(butText, characterSize_pxls);
	newText.setIsVisible(getIsVisible());
	newText.setColor(col);
	newText.setAbsolutePosition(getAlignedPosition(align, newText.getTextSize()));
	frameTexts.push_back(std::move(std::make_pair(align, std::move(newText))));
}

const std::deque<std::pair<GuiAlign, MgeText>>& MgeFrame::getTextsFromFrame() const noexcept
{
	return frameTexts;
}

void MgeFrame::setIsVisible(bool visible) noexcept
{
	MgeWidget::setIsVisible(visible);

	for (auto& text : frameTexts)
		text.second.setIsVisible(visible);

	if (std::holds_alternative<MgeDrawable>(frameObject))
	{
		auto& obj = std::get<MgeDrawable>(frameObject);
		obj.setIsVisible(visible);

	}
	else if (std::holds_alternative<MgeImage>(frameObject))
	{
		auto& img = std::get<MgeImage>(frameObject);
		img.setVisible(visible);
	}

#ifdef _DEBUG
	else if (std::holds_alternative<UNDEFINED_FRAME_OBJECT>(frameObject))
	{
	} //nothing to draw
	else
		_ASSERT(false); //unhandled
#endif // _DEBUG

	for (auto& child : editChildren())
	{
		if (auto widget = std::dynamic_pointer_cast<MgeWidget>(child))
			widget->setIsVisible(visible);
	}
}

void MgeFrame::setRelativeRotation(float newRotation)
{
	auto difRotation = newRotation - getRelativeRotation();
	if (std::holds_alternative<MgeDrawable>(frameObject))
	{
		auto& obj = std::get<MgeDrawable>(frameObject);
		obj.setRotation(newRotation);
	}
	else if (std::holds_alternative<MgeImage>(frameObject))
	{
		auto& obj = std::get<MgeImage>(frameObject);
		obj.setRotation(newRotation);
	}
	else
	{ 
		_ASSERT(false); //unhandled variant
	} 

	for (auto& col : collisions)
		col.setRotation(col.getRotation() + difRotation);
}

float MgeFrame::getRelativeRotation()
{
	if (std::holds_alternative<MgeDrawable>(frameObject))
	{
		auto& obj = std::get<MgeDrawable>(frameObject);
		return obj.getRotation();
	}
	else if (std::holds_alternative<MgeImage>(frameObject))
	{
		auto& obj = std::get<MgeImage>(frameObject);
		return obj.getRotation();
	}
	else
	{
		_ASSERT(false); //unhandled variant
	}
	return {0.f};
}

void MgeFrame::setOrigin(IPoint newOrigin)
{
	if (std::holds_alternative<MgeDrawable>(frameObject))
	{
		_ASSERT(false); //not yet
	}
	else if (std::holds_alternative<MgeImage>(frameObject))
	{
		auto& img = std::get<MgeImage>(frameObject);
		img.setOrigin(newOrigin.asFloat());
	}
	else
		_ASSERT(false); //unhandled
}

void MgeFrame::setOnCursorOver(Callback_deprecated cursorEnterFunction, Callback_deprecated cursorLeaveFunction)
{
	onCursorEnter = std::move(cursorEnterFunction);
	onCursorLeave = std::move(cursorLeaveFunction);
}

void MgeFrame::layout() noexcept
{
	try
	{
		auto differencePos = getAbsolutePosition() - lastLayoutAbsolutePosition;
		lastLayoutAbsolutePosition = getAbsolutePosition();

		if (editCollision().size() > 0)
		{
			for (auto& col : editCollision())
				col.setAbsolutePosition(col.getAbsolutePosition() + differencePos);
		}

		for (auto& text : frameTexts)
			text.second.setAbsolutePosition(text.second.getAbsolutePosition() + differencePos);

		if (std::holds_alternative<MgeDrawable>(frameObject))
		{
			auto& obj = std::get<MgeDrawable>(frameObject);
			auto difPos = obj.getPosition() + differencePos.asFloat();
			obj.setPosition(difPos);
		}
		else if (std::holds_alternative<MgeImage>(frameObject))
		{
			auto& img = std::get<MgeImage>(frameObject);
			img.setImgAbsolutePosition(getAbsolutePosition().asFloat());
		}

#ifdef _DEBUG
		else if (std::holds_alternative<UNDEFINED_FRAME_OBJECT>(frameObject))
		{
		} //nothing to draw
		else
			_ASSERT(false); //unhandled
#endif // _DEBUG

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

bool MgeFrame::isUnderCursor() const noexcept
{
	return isUnderMouseCursor;
}

IPoint MgeFrame::getAlignedPosition(GuiAlign align, mgeType::Size<int> objectSize)
{
	IPoint framePos = getAbsolutePosition();
	auto frameSize = getSize();

	switch (align)
	{
		case GuiAlign::TopLeft:
			return framePos;
		case GuiAlign::TopCenter:
			_ASSERT(false); //not implemented yet
			break;
		case GuiAlign::TopRight:
			_ASSERT(false); //not implemented yet
			break;
		case GuiAlign::MiddleLeft:
			_ASSERT(false); //not implemented yetq
			break;
		case GuiAlign::MiddleCenter:
		{
			IPoint newPos(framePos.x + frameSize.width / 2 - objectSize.width / 2,
				framePos.y + frameSize.height / 2 - objectSize.height / 2);
			return newPos;
		}
		case GuiAlign::MiddleRight:
			_ASSERT(false); //not implemented yet
			break;
		case GuiAlign::BottomLeft:
			_ASSERT(false); //not implemented yet
			break;
		case GuiAlign::BottomCenter:
			_ASSERT(false); //not implemented yet
			break;
		case GuiAlign::BottomRight:
		{
			IPoint newPos(framePos.x + frameSize.width - objectSize.width,
				framePos.y + frameSize.height - objectSize.height);
			return newPos;
		}
		default:
			_ASSERT(false); //unknown alignment
			break;
	}

	return IPoint();
}
