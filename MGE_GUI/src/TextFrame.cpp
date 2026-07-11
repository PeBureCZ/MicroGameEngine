#include "TextFrame.h"

#include "MlGameWrapper.h"

#include "GuiDependencies.h"

TextFrame::TextFrame(IPoint newPosition, tsmType::Size<int> newSize)
	: Frame(newPosition, newSize)
{

}

void TextFrame::addTextLine(std::string text, unsigned int textPxlsSize, tsmType::Color_RGBA color, bool bold) noexcept
{
	auto newText = ML_wrapper::getGlobalGameWrapper()->createText(std::move(text), textPxlsSize, bold);
	if (newText.has_value())
	{
		auto& usedText = newText.value();
		usedText.setColor(std::move(color));
		frameTexts.push_back(std::make_pair(GuiAlign::TopLeft, std::move(usedText)));
		redrawTextFrame();
	}
}

void TextFrame::setPadding(int leftBorder_pxls, int topBorder_pxls, int betweenTextLine_pxls)
{
	leftPadding = leftBorder_pxls;
	topPadding = topBorder_pxls;
	linePadding_pxls = betweenTextLine_pxls;
	redrawTextFrame();
}

void TextFrame::removeLine(size_t index) noexcept
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

void TextFrame::removeFirstLine() noexcept
{
	if (!frameTexts.empty())
		frameTexts.pop_front();
}

void TextFrame::removeLastLine() noexcept
{
	if (!frameTexts.empty())
		frameTexts.pop_back();
}

void TextFrame::setTextsAlign(GuiAlign align)
{
	usedAlign = align;
}

size_t TextFrame::getAllTextsHeight() noexcept
{
	size_t allTextHeight_pxls = 0;
	for (const auto& text : frameTexts)
		allTextHeight_pxls += (size_t)text.second.getTextSize().height + linePadding_pxls;
	return allTextHeight_pxls;
}

void TextFrame::redrawTextFrame() noexcept
{
	if (frameTexts.empty())
		return;

	try
	{
		IPoint actualAbsolutePos_pxls = getAbsolutePosition();
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
			text.second.setAbsolutePosition(actualAbsolutePos_pxls);
			actualAbsolutePos_pxls.y += linePadding_pxls + text.second.getTextSize().height;
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
