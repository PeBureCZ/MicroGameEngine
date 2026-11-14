#include "MgeRollMenu.h"

#include "MlWrapper.h"

MgeRollMenu::MgeRollMenu(const FPoint& newPosition, ISize newSize, std::string_view text)
	: MgeFrame(newPosition, newSize)
{
	m_mainButtonText = text;
}

mge::Button MgeRollMenu::addRollButton(const std::string& butText, VoidCallback onLMBClickFunc, VoidCallback onRMBClickFunc)
{
	getMainButton(); //generate main but if not exist

	size_t relativeYpos_pxls = getSize().height * (m_rollButtons.size() + 1);
	auto newButton = mge::createButton(FPoint(0.f, (float)relativeYpos_pxls), getSize());
	newButton->setOnLMBClick(std::move(onLMBClickFunc));
	newButton->setOnRMBClick(std::move(onRMBClickFunc));
	newButton->setOnCursorOver(nullptr, [this]() noexcept { onMenuItemCursorLeave(); }); //auto close
	newButton->setButtonText(butText, 14, GuiAlign::MiddleCenter);

	addWidget(newButton);
	newButton->setIsVisible(m_isRolled && getIsVisible());
	m_rollButtons.push_back(newButton);
	return newButton;
}

mge::Button MgeRollMenu::getMainButton()
{
	if (!m_mainButton)
	{
		m_mainButton = mge::createButton(FPoint(0.f, 0.f), getSize());
		m_mainButton->setColor(DEFAULT_FRAME_COLOR);
		m_mainButton->setHoveredColor(MOUSE_OVER_FRAME_COLOR);
		m_mainButton->setButtonTextColors(DEFAULT_TEXT_COLOR, DEFAULT_TEXT_COLOR, DISABLED_TEXT_COLOR,
			DEFAULT_TEXT_COLOR, DEFAULT_TEXT_COLOR);
		m_mainButton->setOnLMBClick([this]() { onRollMenuLMBClick(); });
		m_mainButton->setOnCursorOver(
			[this]() noexcept { onCursorEnterCall(); },     //auto open - if it is set
			[this]() noexcept { onMenuItemCursorLeave(); }); //auto close - if it is set
		m_mainButton->setButtonText(m_mainButtonText, 14, GuiAlign::MiddleCenter);
		addWidget(m_mainButton);
		m_mainButton->setIsVisible(getIsVisible());
	}
	return m_mainButton;
}

void MgeRollMenu::setMainButtonText(std::string_view text)
{
	m_mainButtonText = text;
	if (m_mainButton)
		m_mainButton->setButtonText(m_mainButtonText);
}

void MgeRollMenu::open() noexcept
{
	if (!m_isRolled)
		openOrCloseMenu(true);
}

void MgeRollMenu::close() noexcept
{
	if (m_isRolled)
		openOrCloseMenu(false);
}

bool MgeRollMenu::isOpen() const noexcept
{
	return m_isRolled;
}

void MgeRollMenu::setAutoOpen(bool newAutoOpen) noexcept
{
	m_autoOpen = newAutoOpen;
}

void MgeRollMenu::setAutoClose(bool newAutoClose) noexcept
{
	m_autoClose = newAutoClose;
}

void MgeRollMenu::setIsVisible(bool visible) noexcept
{
	MgeFrame::setIsVisible(visible); //shows/hides all children, including the rolled buttons
	for (auto& button : m_rollButtons)
	{
		_ASSERT(button.lock());
		if (auto but = button.lock())
			but->setIsVisible(visible && m_isRolled); //a hidden (closed) menu must stay closed when shown again
	}
}

void MgeRollMenu::onCursorLeaveCall() noexcept
{
	//the menu frame itself is (usually) fully covered by its main button, so this runs rarely;
	// the leave callbacks of the buttons handle auto close
	onMenuItemCursorLeave();
	MgeFrame::onCursorLeaveCall();
}

void MgeRollMenu::onCursorEnterCall() noexcept
{
	if (m_autoOpen)
		openOrCloseMenu(true/* = open menu*/);
	MgeFrame::onCursorEnterCall();
}

void MgeRollMenu::onMenuItemCursorLeave() noexcept
{
	//leave of one item: moving from the main button onto a row (or between rows) must not close the menu
	if (m_autoClose && m_isRolled && !isCursorOverMenu())
		openOrCloseMenu(false/* = close menu*/);
}

bool MgeRollMenu::isCursorOverMenu() const noexcept
{
	try
	{
		auto mlWrapper = ML_wrapper::getGlobalMlWrapper();
		if (!mlWrapper)
			return false;
		const IPoint& cursorPos = mlWrapper->getCursorGuiPosition();

		auto isOver = [&cursorPos](const MgeButton& button) -> bool
			{
				if (!button.getIsVisible())
					return false;
				for (const auto& col : button.getCollisions())
				{
					if (col.isPointInside(cursorPos)) //false for disabled collisions
						return true;
				}
				return false;
			};

		if (m_mainButton && isOver(*m_mainButton))
			return true;
		for (const auto& button : m_rollButtons)
		{
			if (auto but = button.lock(); but && isOver(*but))
				return true;
		}
	}
	catch (...)
	{
		_ASSERT(false);
	}
	return false;
}

void MgeRollMenu::openOrCloseMenu(bool open) noexcept
{
	m_isRolled = open;
	const bool showRows = open && getIsVisible();
	for (auto& button : m_rollButtons)
	{
		_ASSERT(button.lock()); //wrong ptr management
		if (auto but = button.lock())
			but->setIsVisible(showRows);
	}
}

void MgeRollMenu::onRollMenuLMBClick()
{
	openOrCloseMenu(!m_isRolled);
}
