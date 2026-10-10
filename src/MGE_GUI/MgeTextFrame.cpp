#include "MgeTextFrame.h"

#include "MlWrapper.h"
#include "GlobalFunctions.h"
#include "GraphicDependencies.h"

MgeTextFrame::MgeTextFrame(const FPoint& newPosition, mgeType::Size<int> newSize, std::string text, unsigned int characterSize_pxls,
	GraphicItemLayer layer, MgeColor textColor, MgeColor frameColor)
	: MgeFrame(newPosition, newSize, layer)
{
	setColor(frameColor);
	if (!text.empty())
	{
		MgeText newText(std::move(text), characterSize_pxls, FPoint(), false, layer, textColor);
		m_frameTexts.push_back(std::move(newText));
	}
}

unsigned int MgeTextFrame::getDefaultNewTextSize() const noexcept
{
	return m_defaultNewTextSize_pxls;
}

void MgeTextFrame::setDefaultNewTextSize(unsigned int height_pxls) noexcept
{
	m_defaultNewTextSize_pxls = height_pxls;
}

void MgeTextFrame::addTextLine(std::string text, unsigned int height_pxls, MgeColor color, bool bold) noexcept
{
	size_t layer = getLayer();
	MgeText newText(std::move(text), height_pxls, FPoint(), bold, layer);
	newText.setColor(std::move(color));
	m_frameTexts.push_back(std::move(newText));
	redrawTextFrame();
}

void MgeTextFrame::appendTextToLine(const std::string& text_utf8, size_t lineIndex)
{
	_ASSERT(lineIndex < m_frameTexts.size());
	if (lineIndex < m_frameTexts.size())
		m_frameTexts[lineIndex].appendText(text_utf8);
}

const std::deque<MgeText>& MgeTextFrame::getTextLines() const
{
	return m_frameTexts;
}

std::deque<MgeText>& MgeTextFrame::editTextLines()
{
	return m_frameTexts;
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
	if (index < m_frameTexts.size())
	{
		m_frameTexts.erase(m_frameTexts.begin() + index);
		redrawTextFrame();
	}
	else
	{
		_ASSERT(false); //try to remove index out of range
	}
}

void MgeTextFrame::removeFirstLine() noexcept
{
	if (!m_frameTexts.empty())
		m_frameTexts.pop_front();
}

void MgeTextFrame::removeLastLine() noexcept
{
	if (!m_frameTexts.empty())
		m_frameTexts.pop_back();
}

void MgeTextFrame::layout() noexcept
{
	if (m_frameTexts.size() > 0)
	{
		auto differencePos = getAbsolutePosition() - lastLayoutAbsolutePosition;
		const auto sizeChanged = (lastLayoutSize != getSize());

		for (auto& text : m_frameTexts)
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

void MgeTextFrame::setTextColor(MgeColor newColor, size_t lineIndex)
{
	_ASSERT(lineIndex < m_frameTexts.size());
	if (lineIndex < m_frameTexts.size())
	{
		m_frameTexts[lineIndex].setColor(newColor);
		redrawTextFrame();
	}
}

size_t MgeTextFrame::getAllTextsHeight() noexcept
{
	size_t allTextHeight_pxls = 0;
	for (const auto& text : m_frameTexts)
		allTextHeight_pxls += (size_t)text.getTextSize().height + linePadding_pxls;
	return allTextHeight_pxls;
}

void MgeTextFrame::redrawTextFrame() noexcept
{
	if (m_frameTexts.empty())
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

		for (auto& text : m_frameTexts)
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
		GraphicItemLayer layer, const MgeColor& textColor, const MgeColor& frameColor)
	{
		auto newTextFrame = std::make_shared<MgeTextFrame>(position, std::move(size), text, characterSize_pxls, layer, textColor, frameColor);
		newTextFrame->initializeSelf(newTextFrame);
		return newTextFrame;
	}

	TextFrame createTextFrame(const FPoint& newPosition, mgeType::Size<int> newSize, const std::wstring& text, unsigned int characterSize_pxls, GraphicItemLayer layer, const MgeColor& textColor, const MgeColor& color)
	{
		std::string utf8text = mgeCore::toUTF8(text);
		return createTextFrame(newPosition, newSize, std::move(utf8text), characterSize_pxls, layer, textColor, color);
	}
}
