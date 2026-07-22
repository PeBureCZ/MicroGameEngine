#include "Frame.h"

#include "BasicShapes.h"
#include "BasicTypes.h"
#include "MlVerticesObject.h"
#include "MlGameWrapper.h"
#include "GraphicDependencies.h"
#include "Trigger.h"
#include "MlText.h"



Frame::Frame(const IPoint& newPosition, const ISize& newSize)
	: Widget(newPosition, newSize)
{
	DrawableObject<float> drawable = DrawableObject<float>(mgeShape::Rectangle<float>(newPosition.asFloat(), mgeType::Size<float>((float)newSize.width, (float)newSize.height)), mgeType::Color_RGBA(100, 100, 100, 150));
	frameObject = drawable;
	auto newWidgetVertices = std::make_shared<MlVerticesObject>(GraphicItemLayer::GUI_LAYER);
	newWidgetVertices->addObjects({ drawable });
	
	setVertices(newWidgetVertices);
}

Frame::Frame(const IPoint& newPosition, TextureId textureId)
	: Widget(newPosition, mgeType::Size<int>())
{
	auto image = std::make_shared<MlImage>(std::move(textureId), GraphicItemLayer::GUI_LAYER, newPosition.asFloat());
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().appendGuiImage(*image, newPosition.asFloat());
	setSize(mgeType::Size<int>(ML_wrapper::getGlobalGameWrapper()->getImageHolder().getTextureSize(image->getTextureID())));
	frameObject = std::move(image);
}

void Frame::setImage(TextureId textureId)
{
	auto image = std::make_shared<MlImage>(std::move(textureId), GraphicItemLayer::GUI_LAYER, getAbsolutePosition().asFloat());
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().appendGuiImage(*image, getAbsolutePosition().asFloat());
	setSize(mgeType::Size<int>(ML_wrapper::getGlobalGameWrapper()->getImageHolder().getTextureSize(image->getTextureID())));
	editCollision().clear();
	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition(), getSize())));
	frameObject = std::move(image);
}

void Frame::setVertices(const std::shared_ptr<MlVerticesObject>& newVertices) noexcept
{
	if (drawableItem)
		ML_wrapper::getGlobalGameWrapper()->removeMlVerticesObject(drawableItem, drawableItem->getLayer());
	auto id = ML_wrapper::getGlobalGameWrapper()->addMlVerticesObject(newVertices);
	drawableItem = newVertices;
}

std::optional<std::shared_ptr<MlVerticesObject>> Frame::getVertices()
{
	if (drawableItem)
		return drawableItem;
	return std::nullopt;
}

std::vector<Trigger<int>>& Frame::editCollision() noexcept
{
	return collisions;
}

const std::vector<Trigger<int>>& Frame::getCollision() const noexcept
{
	return collisions;
}

void Frame::addCollision(Trigger<int> addedCollision)
{
	auto difPos_pxl = (getParent()) ? getRelativePosition() : getAbsolutePosition();
	addedCollision.setAbsolutePosition(addedCollision.getAbsolutePosition() + difPos_pxl);
	collisions.push_back(std::move(addedCollision));
}

void Frame::setColor(mgeType::Color_RGBA newColor)
{
	if (std::holds_alternative<DrawableObject<float>>(frameObject))
	{
		auto& drawableObject = std::get<DrawableObject<float>>(frameObject);
		drawableObject.setColor(newColor);
		if (drawableItem)
			ML_wrapper::getGlobalGameWrapper()->setMlVerticesColor(drawableItem->getUniqueId(), std::move(newColor));
	}
	else if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
	{
		auto& img = std::get<std::shared_ptr<MlImage>>(frameObject);
		img->setColor(newColor);
	}
	else
	{
		_ASSERT(false); //unhandled
	}

}

const FRAME_OBJECT& Frame::getFrameObject() const noexcept
{
	return frameObject;
}

size_t Frame::getVerticesId() const noexcept
{
	if (drawableItem)
		return drawableItem->getUniqueId();
	return 0;
}

void Frame::addTextToFrame(MlText newText, GuiAlign align)
{
	newText.setAbsolutePosition(getAlignedPosition(align, newText.getTextSize()));
	frameTexts.push_back(std::make_pair(align, std::move(newText)));
}

const std::deque<std::pair<GuiAlign, MlText>>& Frame::getTextsFromFrame() const noexcept
{
	return frameTexts;
}

void Frame::setRelativeRotation(float newRotation)
{
	auto difRotation = newRotation - getRelativeRotation();
	if (std::holds_alternative<DrawableObject<float>>(frameObject))
	{
		auto& obj = std::get<DrawableObject<float>>(frameObject);
		obj.setRotation(newRotation);
		auto usedVertices_opt = getVertices();
		if (usedVertices_opt.has_value() && usedVertices_opt.value())
			ML_wrapper::getGlobalGameWrapper()->setMlVerticesRotation(usedVertices_opt.value(), newRotation);
	}
	else if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
	{
		if (auto& obj = std::get<std::shared_ptr<MlImage>>(frameObject))
			obj->setRotation(newRotation);
	}
	else if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
	{} //nothing to do

	for (auto& col : collisions)
		col.setRotation(col.getRotation() + difRotation);
}

float Frame::getRelativeRotation()
{
	if (std::holds_alternative<DrawableObject<float>>(frameObject))
	{
		auto& obj = std::get<DrawableObject<float>>(frameObject);
		return obj.getRotation();
	}
	else if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
	{
		if (auto& obj = std::get<std::shared_ptr<MlImage>>(frameObject))
			return obj->getRotation();
	}
	else if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
	{
	} //nothing to do
	return {0.f};
}

void Frame::setOrigin(IPoint newOrigin)
{
	if (std::holds_alternative<DrawableObject<float>>(frameObject))
		_ASSERT(false); //not yet
	else if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
	{
		if (auto& img = std::get<std::shared_ptr<MlImage>>(frameObject))
			img->setOrigin(newOrigin.asFloat());
	}
	else
		_ASSERT(false); //unhandled
}

void Frame::setOnCursorOver(Callback cursorEnterFunction, Callback cursorLeaveFunction)
{
	onCursorEnter = std::move(cursorEnterFunction);
	onCursorLeave = std::move(cursorLeaveFunction);
}

void Frame::layout() noexcept
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

		if (std::holds_alternative<DrawableObject<float>>(frameObject))
		{
			auto& obj = std::get<DrawableObject<float>>(frameObject);
			auto difPos = obj.getPosition() + differencePos.asFloat();
			obj.setAbsoluteDrawablePosition(difPos.asFloat());

			auto usedVertices_opt = getVertices();
			if (usedVertices_opt.has_value() && usedVertices_opt.value())
				ML_wrapper::getGlobalGameWrapper()->moveMlVerticesPosition(usedVertices_opt.value(), differencePos.asFloat());
		}
		else if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
		{
			auto& img = std::get<std::shared_ptr<MlImage>>(frameObject);
			if (img)
				img->setImgAbsolutePosition(getAbsolutePosition().asFloat());
		}

#ifdef _DEBUG
		else if (std::holds_alternative<UNDEFINED_FRAME_OBJECT>(frameObject))
		{
		} //nothing to draw
		else
			_ASSERT(false); //unhandled
#endif // _DEBUG

		Widget::layout();
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

void Frame::onCursorEnterCall() noexcept
{
	if (onCursorEnter)
		onCursorEnter();
}

void Frame::onCursorLeaveCall() noexcept
{
	if (onCursorLeave)
		onCursorLeave();
}

void Frame::setUnderMouseCursor(bool isUnderMouse)
{
	if (!isUnderMouseCursor && isUnderMouse)
		onCursorEnterCall();
	else if (isUnderMouseCursor && !isUnderMouse)
		onCursorLeaveCall();
	isUnderMouseCursor = isUnderMouse;
}

bool Frame::isUnderCursor() const noexcept
{
	return isUnderMouseCursor;
}

IPoint Frame::getAlignedPosition(GuiAlign align, mgeType::Size<int> objectSize)
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
