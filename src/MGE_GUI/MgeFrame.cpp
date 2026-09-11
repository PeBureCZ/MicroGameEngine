#include "MgeFrame.h"

#include "BasicShapes.h"
#include "BasicTypes.h"
#include "MgeDrawable.h"
#include "MlWrapper.h"
#include "GraphicDependencies.h"
#include "Trigger.h"
#include "MgeText.h"


MgeFrame::MgeFrame(const FPoint& newPosition, const ISize& newSize)
	: MgeWidget(newPosition, newSize)
{
	const MgeVertices<float> drawable(mgeShape::Rectangle<float>(FPoint(0,0), mgeType::Size<float>((float)newSize.width, (float)newSize.height)), mgeType::Color_RGBA(100, 100, 100, 150));
	MgeDrawable newWidgetVertices(drawable, newPosition.asFloat(), 0.f, GraphicItemLayer::GUI_LAYER);
	frameObject = std::move(newWidgetVertices);
}

MgeFrame::MgeFrame(const FPoint& newPosition, TextureId textureId)
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
	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition().asInt(), getSize())));
}

void MgeFrame::setImage(MgeImage&& image)
{
	_ASSERT(image.getLayer() == GraphicItemLayer::GUI_LAYER); //image should be in GUI layer to avoid unrelated issues
	setSize(image.getSize());
	frameObject = std::move(image);
	editCollision().clear();
	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition().asInt(), getSize())));
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
	addedCollision.setAbsolutePosition(addedCollision.getAbsolutePosition() + difPos_pxl.asInt());
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

void MgeFrame::setBorder(BorderSide sides, unsigned int width_pxls, const mgeType::Color_RGBA& color)
{
	auto flags = static_cast<uint8_t>(sides);
	m_borderFlags = sides;
	m_borderWidth_pxls = width_pxls;
	m_borderColor = color;

	MgeDrawable borders(GraphicItemLayer::GUI_LAYER);

	std::vector<MgeVertices<float>> vecVertices;
	ISize actualSize = getSize();

	if (flags & static_cast<uint8_t>(BorderSide::None))
	{
		borderObject = false;
		return;
	}

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

	borders.addObjects(std::move(vecVertices));
	borders.setPosition(getAbsolutePosition());

	if (std::holds_alternative<MgeDrawable>(borderObject))
	{
		auto& drawable = std::get<MgeDrawable>(borderObject);
		int x = 0;
	}

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

		for (auto& text : frameTexts)
		{
			if (sizeChanged)
				text.second.setAbsolutePosition(getAlignedPosition(text.first, text.second.getTextSize()));
			else
				text.second.setAbsolutePosition(text.second.getAbsolutePosition() + differencePos.asInt());
		}

		//BORDERS
		if (std::holds_alternative<MgeDrawable>(borderObject))
			setBorder(m_borderFlags, m_borderWidth_pxls, m_borderColor); //re-draw border with aktual size
		else if (std::holds_alternative<bool>(borderObject))
		{} //nothing to re-draw
		else
		{
			_ASSERT(false); //unwantend behaviour
		}

		//FRAME OBJECT
		if (std::holds_alternative<MgeDrawable>(frameObject))
		{
			auto& obj = std::get<MgeDrawable>(frameObject);
			auto difPos = obj.getPosition() + differencePos.asFloat();
			obj.setPosition(difPos);

			if (sizeChanged)
				obj.rescale(scaleX, scaleY);
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
