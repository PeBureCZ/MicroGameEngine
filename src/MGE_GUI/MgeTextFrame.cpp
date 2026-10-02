#include "MgeTextFrame.h"

#include "MlWrapper.h"

#include "GraphicDependencies.h"

MgeTextFrame::MgeTextFrame(const FPoint& newPosition, mgeType::Size<int> newSize, std::string text, unsigned int characterSize_pxls,
	GraphicItemLayer layer, MgeColor textColor, MgeColor frameColor)
	: MgeFrame(newPosition, newSize)
{
	setColor(frameColor);
	if (!text.empty())
	{
		MgeText newText(std::move(text), characterSize_pxls, FPoint(), false, layer);
		frameTexts.push_back(std::move(newText));
	}
}

void MgeTextFrame::addTextLine(std::string text, unsigned int textPxlsSize, MgeColor color, bool bold) noexcept
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
	redrawTextFrame();
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

//void MgeTextFrame::redrawTextFrame() noexcept
//{
//	if (frameTexts.empty())
//		return;
//
//	try
//	{
//		IPoint actualAbsolutePos_pxls = getAbsolutePosition().asInt();
//		actualAbsolutePos_pxls.y += topPadding;
//		actualAbsolutePos_pxls.x += leftPadding;
//
//		switch (usedAlign)
//		{
//		case GuiAlign::TopLeft: break;	//used as default
//		case GuiAlign::TopCenter: _ASSERT(false); break; //not yet
//		case GuiAlign::TopRight: _ASSERT(false); break; //not yet
//		case GuiAlign::MiddleLeft: _ASSERT(false); break; //not yet
//		case GuiAlign::MiddleCenter: _ASSERT(false); break; //not yet
//		case GuiAlign::MiddleRight: _ASSERT(false); break; //not yet
//		case GuiAlign::BottomLeft:
//		{
//			actualAbsolutePos_pxls.y += getSize().height - static_cast<int>(getAllTextsHeight());
//			break;
//		}
//		case GuiAlign::BottomCenter: _ASSERT(false); break; //not yet
//		case GuiAlign::BottomRight: _ASSERT(false); break; //not yet
//		default: {}
//		}
//
//		for (auto& text : frameTexts)
//		{
//			text.setAbsolutePosition(actualAbsolutePos_pxls);
//			actualAbsolutePos_pxls.y += linePadding_pxls + text.getTextSize().height;
//		}
//	}
//#ifdef _DEBUG
//	catch ([[maybe_unused]] const std::exception& e)
//	{
//		_ASSERT(false);
//	}
//#endif
//	catch (...)
//	{
//		_ASSERT(false);
//	}
//}

void MgeTextFrame::redrawTextFrame() noexcept
{
	if (frameTexts.empty())
		return;

	try
	{
		const auto framePos = getAbsolutePosition().asInt();
		const auto frameSize = getSize();

		const int allTextsHeight = static_cast<int>(getAllTextsHeight());

		// Calculate vertical position of the whole text block.
		int textBlockY = framePos.y;

		switch (usedAlign)
		{
		case GuiAlign::TopLeft:
		case GuiAlign::TopCenter:
		case GuiAlign::TopRight:
			textBlockY += topPadding;
			break;

		case GuiAlign::MiddleLeft:
		case GuiAlign::MiddleCenter:
		case GuiAlign::MiddleRight:
			textBlockY += (frameSize.height - allTextsHeight) / 2;
			break;

		case GuiAlign::BottomLeft:
		case GuiAlign::BottomCenter:
		case GuiAlign::BottomRight:
			textBlockY += frameSize.height - allTextsHeight - bottomPadding;
			break;

		default:
			break;
		}

		int actualY = textBlockY;

		for (auto& text : frameTexts)
		{
			const int textWidth = static_cast<int>(text.getTextSize().width);

			int actualX = framePos.x;

			switch (usedAlign)
			{
			case GuiAlign::TopLeft:
			case GuiAlign::MiddleLeft:
			case GuiAlign::BottomLeft:
				actualX += leftPadding;
				break;

			case GuiAlign::TopCenter:
			case GuiAlign::MiddleCenter:
			case GuiAlign::BottomCenter:
				actualX += (frameSize.width - textWidth) / 2;
				break;

			case GuiAlign::TopRight:
			case GuiAlign::MiddleRight:
			case GuiAlign::BottomRight:
				actualX += frameSize.width - textWidth - rightPadding;
				break;

			default:
				break;
			}

			text.setAbsolutePosition({ actualX, actualY });

			actualY += linePadding_pxls + text.getTextSize().height;
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
		GraphicItemLayer layer, MgeColor textColor, MgeColor frameColor)
	{
		auto newTextFrame = std::make_shared<MgeTextFrame>(position, size, text, characterSize_pxls, layer, textColor, frameColor);
		newTextFrame->initializeSelf(newTextFrame);
		return newTextFrame;
	}
}
