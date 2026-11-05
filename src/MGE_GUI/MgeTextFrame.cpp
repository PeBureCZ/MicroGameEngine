#include "MgeTextFrame.h"

#include "MlWrapper.h"
#include "GlobalFunctions.h"
#include "GraphicDependencies.h"
#include "MgeTextValidator.h"

MgeTextFrame::MgeTextFrame(const FPoint& newPosition, const ISize& size, std::string text, unsigned int characterSize_pxls,
	GraphicItemLayer layer, MgeColor textColor, MgeColor frameColor, bool editable)
	: MgeFrame(newPosition, size, layer)
{
	setIsFocusable(editable);
	setEditable(editable);
	setColor(frameColor);
	m_defaultTextColor = textColor;

	if (editable)
		setBasicCollision();

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

void  MgeTextFrame::setDefaultTextColor(const MgeColor& color) noexcept
{
	m_defaultTextColor = color;
}

void MgeTextFrame::addTextLine(std::string text_utf8, unsigned int height_pxls, MgeColor color, bool bold) noexcept
{
	size_t layer = getLayer();
	MgeText newText(std::move(text_utf8), height_pxls, FPoint(), bold, layer, m_defaultTextColor);
	newText.setColor(std::move(color));
	m_frameTexts.push_back(std::move(newText));
	redrawTextFrame();
}

void MgeTextFrame::addTextLine(std::wstring text, unsigned int height_pxls, MgeColor color, bool bold) noexcept
{
	std::string text_utf8 = mgeCore::toUTF8(text);
	addTextLine(std::move(text_utf8), height_pxls, color, bold);
}

void MgeTextFrame::appendTextToLine(const std::string& text_utf8, size_t lineIndex)
{
	_ASSERT(lineIndex < m_frameTexts.size());
	if (lineIndex < m_frameTexts.size())
	{
		m_frameTexts[lineIndex].appendText(text_utf8);
		redrawTextFrame();
	}
}

const std::deque<MgeText>& MgeTextFrame::getTextLines() const
{
	return m_frameTexts;
}

std::deque<MgeText>& MgeTextFrame::editTextLines()
{
	return m_frameTexts;
}

void MgeTextFrame::setPadding(int leftPadding_pxls, int topPadding_pxls, int rightPadding_pxls,
	int bottomPadding_pxls, int betweenTextLine_pxls)
{
	m_leftPadding = leftPadding_pxls;
	m_topPadding = topPadding_pxls;
	m_bottomPadding = bottomPadding_pxls;
	m_rightPadding = rightPadding_pxls;
	linePadding_pxls = betweenTextLine_pxls;
	redrawTextFrame();
}

void MgeTextFrame::setEditable(bool editable, bool defaultFrameColoring) noexcept
{
	setIsFocusable(editable);
	editCollisions().clear();

	if (editable)
		setBasicCollision();

	m_defaultFrameColoring = defaultFrameColoring;
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

void  MgeTextFrame::clear() noexcept
{
	m_frameTexts.clear();
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
	usedAlign = align;
	redrawTextFrame();
}

void MgeTextFrame::setTextLineColor(const MgeColor& newColor, size_t lineIndex)
{
	_ASSERT(lineIndex < m_frameTexts.size());
	if (lineIndex < m_frameTexts.size())
	{
		m_frameTexts[lineIndex].setColor(newColor);
		redrawTextFrame();
	}
}

void MgeTextFrame::setUnderEditColors(const MgeColor& nonEditColor, const MgeColor& underEditColor)
{
	m_nonEditColor = nonEditColor;
	m_underEditColor = underEditColor;
}

void MgeTextFrame::setValidator(std::shared_ptr<MgeTextValidator> newValidator) noexcept
{
	m_validator = std::move(newValidator);
}

std::shared_ptr<MgeTextValidator> MgeTextFrame::editValidator() noexcept
{
	return m_validator;
}

void MgeTextFrame::onFocusGet() noexcept
{
	m_isUnderEdit = true;
	_ASSERT(getEventBindingsCount() == 0);
	bindEvent<KeyPressEvent>([this](const KeyPressEvent& keyPressEvent) { processKeyPressEvent(keyPressEvent); });
	bindEvent<TextKeyEnteredEvent>([this](const TextKeyEnteredEvent& textKeyEnteredEvent) { processTextKeyEnteredEvent(textKeyEnteredEvent); });
	if (m_defaultFrameColoring)
	{
		if (auto graphicComponent = getGraphicComponent())
			graphicComponent->setColor(m_underEditColor, BASIC_GRAPHIC_INDEX);
	}
}

void MgeTextFrame::onFocusLost() noexcept
{
	m_isUnderEdit = false;
	removeAllEventBindings();
	if (m_defaultFrameColoring)
	{
		if (auto graphicComponent = getGraphicComponent())
			graphicComponent->setColor(m_nonEditColor, BASIC_GRAPHIC_INDEX);
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
			textBlockY += m_topPadding;
			break;

		case GuiAlign::MiddleLeft:
		case GuiAlign::MiddleCenter:
		case GuiAlign::MiddleRight:
			textBlockY += (frameSize.height - allTextsHeight) / 2;
			break;

		case GuiAlign::BottomLeft:
		case GuiAlign::BottomCenter:
		case GuiAlign::BottomRight:
			textBlockY += frameSize.height - allTextsHeight - m_bottomPadding;
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
				actualX += m_leftPadding;
				break;

			case GuiAlign::TopCenter:
			case GuiAlign::MiddleCenter:
			case GuiAlign::BottomCenter:
				actualX += (frameSize.width - textWidth) / 2;
				break;

			case GuiAlign::TopRight:
			case GuiAlign::MiddleRight:
			case GuiAlign::BottomRight:
				actualX += frameSize.width - textWidth - m_rightPadding;
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

void MgeTextFrame::processKeyPressEvent(const KeyPressEvent& eventData)
{
	if (eventData.m_key == MgeKeys::MgeKey::Enter)
	{
		addTextLine("", getDefaultNewTextSize(), m_defaultTextColor);
	}
	else if (eventData.m_key == MgeKeys::MgeKey::Backspace || eventData.m_key == MgeKeys::MgeKey::Delete) //todo: Delete should be handled differently
	{
		auto& textLines = editTextLines();
		if (!textLines.empty())
		{
			auto& lastLine = textLines.back();
			if (!lastLine.getText().empty())
				lastLine.removeLastCharacter();
			else
				textLines.pop_back();

			redrawTextFrame();
		}
	}
}

void MgeTextFrame::processTextKeyEnteredEvent(const TextKeyEnteredEvent& eventData)
{
	if (eventData.m_textAsUtf8.empty())
		return;

	if (mgeCore::isControlCharacter(eventData.m_textAsUtf8))
		return;

	if (!isTypedTextAllowed(eventData.m_textAsUtf8))
		return;

	auto& textLines = editTextLines();
	if (textLines.size() == 0)
		addTextLine(eventData.m_textAsUtf8, getDefaultNewTextSize(), m_defaultTextColor);
	else
		textLines.back().appendText(eventData.m_textAsUtf8);

	redrawTextFrame();
}

bool MgeTextFrame::isTypedTextAllowed(const std::string& typedText_utf8) const noexcept
{
	if (!m_validator)
		return true;

	try
	{
		const std::string editedLine_utf8 = (m_frameTexts.empty()) ? std::string() : m_frameTexts.back().getText();
		return m_validator->isAllowed(typedText_utf8, editedLine_utf8);
	}
	catch (...)
	{
		_ASSERT(false); //allocation failed
		return false;
	}
}

namespace mge
{
	TextFrame mge::createTextFrame(const FPoint& position, const ISize& size, std::string text, unsigned int characterSize_pxls,
		GraphicItemLayer layer, const MgeColor& textColor, const MgeColor& frameColor)
	{
		auto newTextFrame = std::make_shared<MgeTextFrame>(position, size, text, characterSize_pxls, layer, textColor, frameColor);
		newTextFrame->initializeSelf(newTextFrame);
		return newTextFrame;
	}

	TextFrame createTextFrame(const FPoint& newPosition, const ISize& size, const std::wstring& text, unsigned int characterSize_pxls, GraphicItemLayer layer, const MgeColor& textColor, const MgeColor& color)
	{
		std::string utf8text = mgeCore::toUTF8(text);
		return createTextFrame(newPosition, size, std::move(utf8text), characterSize_pxls, layer, textColor, color);
	}

	TextFrame createEditableTextFrame(const FPoint& position, const ISize& size, const std::wstring& text, unsigned int characterSize_pxls, GraphicItemLayer layer, const MgeColor& textColor, const MgeColor& color)
	{
		std::string utf8text = mgeCore::toUTF8(text);
		auto newTextFrame = std::make_shared<MgeTextFrame>(position, size, std::move(utf8text), characterSize_pxls, layer, textColor, color, true);
		newTextFrame->initializeSelf(newTextFrame);
		return newTextFrame;
	}
}
