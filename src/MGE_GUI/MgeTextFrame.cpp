#include "MgeTextFrame.h"

#include "MlWrapper.h"

#include "GraphicDependencies.h"

MgeTextFrame::MgeTextFrame(const FPoint& newPosition, mgeType::Size<int> newSize, std::string text, unsigned int characterSize_pxls,
	GraphicItemLayer layer, mgeType::Color_RGBA textColor, mgeType::Color_RGBA frameColor)
	: MgeFrame(newPosition, newSize)
{
	setColor(frameColor);
	if (!text.empty())
	{
		MgeText newText(std::move(text), characterSize_pxls, FPoint(), false, layer);
		frameTexts.push_back(std::move(newText));
	}
}

void MgeTextFrame::addTextLine(std::string text, unsigned int textPxlsSize, mgeType::Color_RGBA color, bool bold) noexcept
{
	MgeText newText(std::move(text), textPxlsSize, bold);
	newText.setColor(std::move(color));
	frameTexts.push_back(std::move(newText));
	redrawTextFrame();
}

void MgeTextFrame::setPadding(int leftBorder_pxls, int topBorder_pxls, int betweenTextLine_pxls)
{
	leftPadding = leftBorder_pxls;
	topPadding = topBorder_pxls;
	linePadding_pxls = betweenTextLine_pxls;
	redrawTextFrame();
}

void MgeTextFrame::removeLine(size_t index) noexcept
{
	if (index < frameTexts.size())
	{
		frameTexts.erase(frameTexts.begin() + index);
		redrawTextFrame();
	}
	else
	{
		_ASSERT(false); //try to remove index out of range
	}
}

void MgeTextFrame::removeFirstLine() noexcept
{
	if (!frameTexts.empty())
		frameTexts.pop_front();
}

void MgeTextFrame::removeLastLine() noexcept
{
	if (!frameTexts.empty())
		frameTexts.pop_back();
}

void MgeTextFrame::layout() noexcept
{
	if (frameTexts.size() > 0)
	{
		auto differencePos = getAbsolutePosition() - lastLayoutAbsolutePosition;
		const auto sizeChanged = (lastLayoutSize != getSize());

		for (auto& text : frameTexts)
		{
			if (sizeChanged)
				text.setAbsolutePosition(getAlignedPosition(usedAlign, text.getTextSize()));
			else
				text.setAbsolutePosition(text.getAbsolutePosition() + differencePos.asInt());
		}
	}
	MgeFrame::layout();
}

void MgeTextFrame::setTextsAlign(GuiAlign align)
{
	redrawTextFrame();
	usedAlign = align;
}

size_t MgeTextFrame::getAllTextsHeight() noexcept
{
	size_t allTextHeight_pxls = 0;
	for (const auto& text : frameTexts)
		allTextHeight_pxls += (size_t)text.getTextSize().height + linePadding_pxls;
	return allTextHeight_pxls;
}

void MgeTextFrame::redrawTextFrame() noexcept
{
	if (frameTexts.empty())
		return;

	try
	{
		IPoint actualAbsolutePos_pxls = getAbsolutePosition().asInt();
		actualAbsolutePos_pxls.y += topPadding;
		actualAbsolutePos_pxls.x += leftPadding;

		switch (usedAlign)
		{
		case GuiAlign::TopLeft: break;	//used as default
		case GuiAlign::TopCenter: _ASSERT(false); break; //not yet
		case GuiAlign::TopRight: _ASSERT(false); break; //not yet
		case GuiAlign::MiddleLeft: _ASSERT(false); break; //not yet
		case GuiAlign::MiddleCenter: _ASSERT(false); break; //not yet
		case GuiAlign::MiddleRight: _ASSERT(false); break; //not yet
		case GuiAlign::BottomLeft:
		{
			actualAbsolutePos_pxls.y += getSize().height - static_cast<int>(getAllTextsHeight());
			break;
		}
		case GuiAlign::BottomCenter: _ASSERT(false); break; //not yet
		case GuiAlign::BottomRight: _ASSERT(false); break; //not yet
		default: {}
		}

		for (auto& text : frameTexts)
		{
			text.setAbsolutePosition(actualAbsolutePos_pxls);
			actualAbsolutePos_pxls.y += linePadding_pxls + text.getTextSize().height;
		}
	}
#ifdef _DEBUG
	catch ([[maybe_unused]] const std::exception& e)
	{
		_ASSERT(false);
	}
#endif
	catch (...)
	{
		_ASSERT(false);
	}
}

namespace mge
{
	TextFrame mge::createTextFrame(const FPoint& position, mgeType::Size<int> size, std::string text, unsigned int characterSize_pxls,
		GraphicItemLayer layer, mgeType::Color_RGBA textColor, mgeType::Color_RGBA frameColor)
	{
		auto newTextFrame = std::make_shared<MgeTextFrame>(position, size, text, characterSize_pxls, layer, textColor, frameColor);
		newTextFrame->initializeSelf(newTextFrame);
		return newTextFrame;
	}
}
