#include "MgeMessageWindow.h"

#include <algorithm>

#include "MgeButton.h"
#include "MgeTextFrame.h"
#include "MlWrapper.h"
#include "GlobalFunctions.h"

namespace
{
	constexpr int MIN_WIN_WIDTH_PXLS = 460;
	constexpr int MARGIN_PXLS = 10;
	constexpr int HEADER_HEIGHT_PXLS = 30;
	constexpr int LABEL_HEIGHT_PXLS = 26;
	constexpr int BUTTON_GAP_PXLS = 10;
	constexpr ISize BUTTON_SIZE_PXLS = ISize(120, 36);
	constexpr FPoint HEADER_TEXT_POSITION_PXLS = FPoint(5.f, 5.f);
	constexpr unsigned int HEADER_TEXT_SIZE_PXLS = 20;
	constexpr unsigned int LABEL_TEXT_SIZE_PXLS = 18;
	constexpr unsigned int BUTTON_TEXT_SIZE_PXLS = 17;
	constexpr unsigned int BACKGROUND_BORDER_PXLS = 2;
	constexpr unsigned int BUTTON_BORDER_PXLS = 2;

	/**keeps only the horizontal part of the align (Top / Bottom -> Middle); the window rows have no vertical align*/
	[[nodiscard]] GuiAlign toHorizontalAlign(GuiAlign align) noexcept
	{
		switch (align)
		{
		case GuiAlign::MiddleLeft:
		case GuiAlign::MiddleCenter:
		case GuiAlign::MiddleRight:
			return align;
		case GuiAlign::TopLeft:
		case GuiAlign::BottomLeft:
			_ASSERT(false); //use MiddleLeft, the vertical part is ignored
			return GuiAlign::MiddleLeft;
		case GuiAlign::TopCenter:
		case GuiAlign::BottomCenter:
			_ASSERT(false); //use MiddleCenter, the vertical part is ignored
			return GuiAlign::MiddleCenter;
		case GuiAlign::TopRight:
		case GuiAlign::BottomRight:
			_ASSERT(false); //use MiddleRight, the vertical part is ignored
			return GuiAlign::MiddleRight;
		default:
			_ASSERT(false); //unknown align
			return GuiAlign::MiddleCenter;
		}
	}

	/**recolors all existing text lines (setTextLineColor() asserts on a missing line, an empty text may have none)*/
	void setAllTextLinesColor(MgeTextFrame& textFrame, const MgeColor& newColor)
	{
		textFrame.setDefaultTextColor(newColor);
		const size_t lineCount = textFrame.getTextLines().size();
		for (size_t i = 0; i < lineCount; ++i)
			textFrame.setTextLineColor(newColor, i);
	}
}

MgeMessageWindow::MgeMessageWindow(std::vector<MessagePart>&& parts, std::wstring headerText)
	: MgeWindow(FPoint(), getWindowSize(parts))
	, m_headerText(std::move(headerText))
	, m_parts(std::move(parts))
{
	MAIN_THREAD_GUARD;
	_ASSERT(std::ranges::any_of(m_parts, isButton)); //without a button the window cannot be closed
}

void MgeMessageWindow::setLabelsAlign(GuiAlign newAlign) noexcept
{
	MAIN_THREAD_GUARD;
	m_labelsAlign = toHorizontalAlign(newAlign);
	if (!hasContent())
		return; //applied by initialize()

	try
	{
		applyLabelsAlign();
	}
	catch (...)
	{
		_ASSERT(false); //unexpected exception
	}
	layout();
}

void MgeMessageWindow::setButtonsAlign(GuiAlign newAlign) noexcept
{
	MAIN_THREAD_GUARD;
	m_buttonsAlign = toHorizontalAlign(newAlign);
	if (!hasContent())
		return; //applied by initialize()

	applyButtonsAlign();
	layout();
}

GuiAlign MgeMessageWindow::getLabelsAlign() const noexcept
{
	return m_labelsAlign;
}

GuiAlign MgeMessageWindow::getButtonsAlign() const noexcept
{
	return m_buttonsAlign;
}

void MgeMessageWindow::setBackgroundColor(const MgeColor& newColor)
{
	MAIN_THREAD_GUARD;
	m_backgroundColor = newColor;
	applyBackgroundColor();
}

MgeColor MgeMessageWindow::getBackgroundColor() const noexcept
{
	return m_backgroundColor;
}

void MgeMessageWindow::setHeaderColor(const MgeColor& newColor)
{
	MAIN_THREAD_GUARD;
	m_headerColor = newColor;
	applyHeaderColor();
}

MgeColor MgeMessageWindow::getHeaderColor() const noexcept
{
	return m_headerColor;
}

void MgeMessageWindow::setHeaderTextColor(const MgeColor& newColor)
{
	MAIN_THREAD_GUARD;
	m_headerTextColor = newColor;
	applyHeaderTextColor();
}

MgeColor MgeMessageWindow::getHeaderTextColor() const noexcept
{
	return m_headerTextColor;
}

void MgeMessageWindow::setDefaultLabelTextColor(const MgeColor& newColor)
{
	MAIN_THREAD_GUARD;
	m_labelTextColor = newColor;
	applyLabelsTextColor();
}

MgeColor MgeMessageWindow::getDefaultLabelTextColor() const noexcept
{
	return m_labelTextColor;
}

void MgeMessageWindow::setDefaultButtonColor(const MgeColor& newColor)
{
	MAIN_THREAD_GUARD;
	m_buttonColor = newColor;
	applyButtonsColors();
}

MgeColor MgeMessageWindow::getDefaultButtonColor() const noexcept
{
	return m_buttonColor;
}

void MgeMessageWindow::setMouseOverButtonColor(const MgeColor& newColor)
{
	MAIN_THREAD_GUARD;
	m_mouseOverButtonColor = newColor;
	applyButtonsColors();
}

MgeColor MgeMessageWindow::getMouseOverButtonColor() const noexcept
{
	return m_mouseOverButtonColor;
}

void MgeMessageWindow::setButtonTextColors(const MgeColor& newDefaultColor, const MgeColor& newMouseOverColor)
{
	MAIN_THREAD_GUARD;
	m_buttonTextColor = newDefaultColor;
	m_mouseOverButtonTextColor = newMouseOverColor;
	applyButtonsColors();
}

MgeColor MgeMessageWindow::getButtonTextColor() const noexcept
{
	return m_buttonTextColor;
}

MgeColor MgeMessageWindow::getMouseOverButtonTextColor() const noexcept
{
	return m_mouseOverButtonTextColor;
}

void MgeMessageWindow::setBorderColor(const MgeColor& newColor)
{
	MAIN_THREAD_GUARD;
	m_borderColor = newColor;
	applyBorderColor();
}

MgeColor MgeMessageWindow::getBorderColor() const noexcept
{
	return m_borderColor;
}

void MgeMessageWindow::initialize() noexcept
{
	MAIN_THREAD_GUARD;
	_ASSERT(!hasContent()); //initialize() is called only once, from addWidget()

	if (hasContent())
		return;

	try
	{
		auto winParent = std::dynamic_pointer_cast<MgeWidget>(getParent());
		_ASSERT(winParent);
		if (!winParent)
			return;

		const auto size = getSize();
		const auto renderWinSize = ML_wrapper::getGlobalMlWrapper()->getRenderWindowSize();
		setAbsolutePosition(IPoint(renderWinSize.width / 2 - size.width / 2, renderWinSize.height / 2 - size.height / 2).asFloat());

		createBackgroundWithHeader();

		float y = static_cast<float>(HEADER_HEIGHT_PXLS + MARGIN_PXLS);
		for (size_t i = 0; i < m_parts.size(); )
		{
			const size_t rowEnd = getRowEnd(m_parts, i);
			if (isButton(m_parts[i]))
			{
				createButtonRow(i, rowEnd, y);
				y += static_cast<float>(BUTTON_SIZE_PXLS.height);
			}
			else
			{
				createLabel(std::get<MgeMessageLabel>(m_parts[i]), y);
				y += static_cast<float>(LABEL_HEIGHT_PXLS);
			}
			y += static_cast<float>(MARGIN_PXLS);
			i = rowEnd;
		}
		m_parts.clear(); //texts and callbacks are in the widgets / m_onClicks now

		applyStyle();
		layout();
	}
	catch ([[maybe_unused]] const std::exception& e)
	{
		_ASSERT(false); //unexpected exception
	}
	catch (...)
	{
		_ASSERT(false); //unexpected exception
	}
}

ISize MgeMessageWindow::getWindowSize(const std::vector<MessagePart>& parts) noexcept
{
	MAIN_THREAD_GUARD;
	int width = MIN_WIN_WIDTH_PXLS;
	int height = HEADER_HEIGHT_PXLS + MARGIN_PXLS;
	for (size_t i = 0; i < parts.size(); )
	{
		const size_t rowEnd = getRowEnd(parts, i);
		if (isButton(parts[i]))
		{
			width = (std::max)(width, getButtonRowWidth(rowEnd - i) + 2 * MARGIN_PXLS);
			height += BUTTON_SIZE_PXLS.height;
		}
		else
			height += LABEL_HEIGHT_PXLS;
		height += MARGIN_PXLS;
		i = rowEnd;
	}
	return ISize(width, height);
}

size_t MgeMessageWindow::getRowEnd(const std::vector<MessagePart>& parts, size_t firstIndex) noexcept
{
	MAIN_THREAD_GUARD;
	if (firstIndex >= parts.size())
		return parts.size();

	if (!isButton(parts[firstIndex]))
		return firstIndex + 1;

	size_t rowEnd = firstIndex + 1;
	while (rowEnd < parts.size() && isButton(parts[rowEnd]))
		++rowEnd;
	return rowEnd;
}

bool MgeMessageWindow::isButton(const MessagePart& part) noexcept
{
	MAIN_THREAD_GUARD;
	return std::holds_alternative<MgeMessageButton>(part);
}

int MgeMessageWindow::getButtonRowWidth(size_t buttonCount) noexcept
{
	if (buttonCount == 0)
		return 0;
	const int count = static_cast<int>(buttonCount);
	return count * BUTTON_SIZE_PXLS.width + (count - 1) * BUTTON_GAP_PXLS;
}

float MgeMessageWindow::getButtonRowX(size_t buttonCount) const noexcept
{
	const int rowWidth = getButtonRowWidth(buttonCount);
	const int windowWidth = getSize().width;
	if (m_buttonsAlign == GuiAlign::MiddleLeft)
		return static_cast<float>(MARGIN_PXLS);
	if (m_buttonsAlign == GuiAlign::MiddleRight)
		return static_cast<float>(windowWidth - MARGIN_PXLS - rowWidth);
	return static_cast<float>((windowWidth - rowWidth) / 2);
}

bool MgeMessageWindow::hasContent() const noexcept
{
	return static_cast<bool>(m_windowBackground);
}

void MgeMessageWindow::createBackgroundWithHeader()
{
	MAIN_THREAD_GUARD;
	const auto size = getSize();
	m_windowBackground = mge::createFrame(FPoint(), size, GraphicItemLayer::WINDOW_LAYER);
	addWidget(m_windowBackground);

	auto headerFrame = mge::createFrame(FPoint(), ISize(size.width, HEADER_HEIGHT_PXLS), GraphicItemLayer::WINDOW_LAYER);
	m_headerFrame = headerFrame;
	m_windowBackground->addWidget(std::move(headerFrame));

	auto headerTextFrame = mge::createTextFrame(HEADER_TEXT_POSITION_PXLS, ISize(1, 1), m_headerText, HEADER_TEXT_SIZE_PXLS,
		GraphicItemLayer::WINDOW_LAYER, m_headerTextColor);
	m_headerTextFrame = headerTextFrame;
	m_windowBackground->addWidget(std::move(headerTextFrame));
}

void MgeMessageWindow::createLabel(const MgeMessageLabel& label, float y)
{
	MAIN_THREAD_GUARD;
	const ISize labelSize(getSize().width - 2 * MARGIN_PXLS, LABEL_HEIGHT_PXLS);
	auto labelFrame = mge::createTextFrame(FPoint(static_cast<float>(MARGIN_PXLS), y), labelSize, label.m_text,
		LABEL_TEXT_SIZE_PXLS, GraphicItemLayer::WINDOW_LAYER, m_labelTextColor);
	m_labels.push_back(labelFrame);
	m_windowBackground->addWidget(std::move(labelFrame));
}

void MgeMessageWindow::createButtonRow(size_t firstIndex, size_t endIndex, float y)
{
	MAIN_THREAD_GUARD;
	m_buttonRows.push_back(ButtonRow{ .m_firstButtonIndex = m_buttons.size(), .m_buttonCount = endIndex - firstIndex, .m_y_pxls = y });

	for (size_t i = firstIndex; i < endIndex; ++i)
	{
		auto& buttonPart = std::get<MgeMessageButton>(m_parts[i]);
		auto button = mge::createButton(FPoint(0.f, y), BUTTON_SIZE_PXLS, GraphicItemLayer::WINDOW_LAYER);
		button->setButtonText(mgeCore::toUTF8(buttonPart.m_text), BUTTON_TEXT_SIZE_PXLS, GuiAlign::MiddleCenter);

		const size_t buttonIndex = m_buttons.size();
		button->setOnLMBClick([this, buttonIndex]() { onButtonClick(buttonIndex); });
		m_buttons.push_back(button);
		m_onClicks.push_back(std::move(buttonPart.m_onClick));
		m_windowBackground->addWidget(std::move(button));
	}
}

void MgeMessageWindow::applyStyle() noexcept
{
	MAIN_THREAD_GUARD;
	applyLabelsAlign();
	applyButtonsAlign();
	applyBackgroundColor();
	applyHeaderColor();
	applyHeaderTextColor();
	applyLabelsTextColor();
	applyButtonsColors();
	applyBorderColor();
}

void MgeMessageWindow::applyLabelsAlign() noexcept
{
	MAIN_THREAD_GUARD;
	forEachLabel([this](MgeTextFrame& label) { label.setTextsAlign(m_labelsAlign); });
}

void MgeMessageWindow::applyButtonsAlign() noexcept
{
	MAIN_THREAD_GUARD;
	for (const auto& row : m_buttonRows)
	{
		_ASSERT(row.m_firstButtonIndex + row.m_buttonCount <= m_buttons.size());
		const size_t rowEnd = (std::min)(row.m_firstButtonIndex + row.m_buttonCount, m_buttons.size());

		float x = getButtonRowX(row.m_buttonCount);
		for (size_t i = row.m_firstButtonIndex; i < rowEnd; ++i)
		{
			if (auto button = m_buttons[i].lock())
				button->setRelativePosition(FPoint(x, row.m_y_pxls));
			x += static_cast<float>(BUTTON_SIZE_PXLS.width + BUTTON_GAP_PXLS);
		}
	}
}

void MgeMessageWindow::applyBackgroundColor() noexcept
{
	MAIN_THREAD_GUARD;
	if (!m_windowBackground)
		return;

	m_windowBackground->setColor(m_backgroundColor);
}

void MgeMessageWindow::applyHeaderColor() noexcept
{
	MAIN_THREAD_GUARD;
	auto headerFrame = m_headerFrame.lock();
	if (!headerFrame)
		return;

	headerFrame->setColor(m_headerColor);
}

void MgeMessageWindow::applyHeaderTextColor() noexcept
{
	MAIN_THREAD_GUARD;
	auto headerTextFrame = m_headerTextFrame.lock();
	if (!headerTextFrame)
		return;

	setAllTextLinesColor(*headerTextFrame, m_headerTextColor);
}

void MgeMessageWindow::applyLabelsTextColor() noexcept
{
	MAIN_THREAD_GUARD;
	forEachLabel([this](MgeTextFrame& label) { setAllTextLinesColor(label, m_labelTextColor); });
}

void MgeMessageWindow::applyButtonsColors() noexcept
{
	MAIN_THREAD_GUARD;
	forEachButton([this](MgeButton& button)
		{
			button.setColor(m_buttonColor);
			button.setHoveredColor(m_mouseOverButtonColor);
			button.setButtonTextColors(m_buttonTextColor, m_mouseOverButtonTextColor, m_buttonTextColor, m_buttonTextColor, m_buttonTextColor);
		});
}

void MgeMessageWindow::applyBorderColor() noexcept
{
	MAIN_THREAD_GUARD;
	if (m_windowBackground)
		m_windowBackground->setBorder(BorderSide::All, BACKGROUND_BORDER_PXLS, m_borderColor);
	forEachButton([this](MgeButton& button) { button.setBorder(BorderSide::All, BUTTON_BORDER_PXLS, m_borderColor); });
}

void MgeMessageWindow::forEachLabel(const std::function<void(MgeTextFrame&)>& action) noexcept
{
	MAIN_THREAD_GUARD;
	for (const auto& weakLabel : m_labels)
	{
		if (auto label = weakLabel.lock())
			action(*label);
	}
}

void MgeMessageWindow::forEachButton(const std::function<void(MgeButton&)>& action) noexcept
{
	MAIN_THREAD_GUARD;
	for (const auto& weakButton : m_buttons)
	{
		if (auto button = weakButton.lock())
			action(*button);
	}
}

void MgeMessageWindow::onButtonClick(size_t buttonIndex)
{
	MAIN_THREAD_GUARD;
	_ASSERT(buttonIndex < m_onClicks.size());
	if (buttonIndex >= m_onClicks.size())
		return;

	std::function<void()> onClick = std::move(m_onClicks[buttonIndex]); //closeMessage() clears m_onClicks
	closeMessage();
	if (onClick)
		onClick();
}

void MgeMessageWindow::closeMessage()
{
	MAIN_THREAD_GUARD;
	// remove lambdas to avoid potentially holding shared pointers
	forEachButton([](MgeButton& button) { button.setOnLMBClick(nullptr); });
	m_onClicks.clear();
	closeWindow();
}
