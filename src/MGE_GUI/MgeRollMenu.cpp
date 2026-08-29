#include "MgeRollMenu.h"

MgeRollMenu::MgeRollMenu(IPoint newPosition, mgeType::Size<int> newSize, std::string text)
	: MgeFrame(newPosition, newSize)
{
	mainButtonText = text;
}

std::shared_ptr<MgeButton> MgeRollMenu::addRollButton(const std::string& butText, Callback_deprecated onLMBClickFunc, Callback_deprecated onRMBClickFunc)
{
	getMainButton(); //generate main but if not exist

	size_t relativeYpos_pxls = getSize().height * (rollButtons.size() + 1);
	auto newButton = std::make_shared<MgeButton>(IPoint(0, (int)relativeYpos_pxls), getSize());
	newButton->setIsVisible(false);
	newButton->setOnLMBClick(onLMBClickFunc);
	newButton->setOnRMBClick(onRMBClickFunc);
	newButton->addTextToFrame(butText, 14);

	mgeGui::addWidgetToGui(getSelfPtr(), newButton);
	rollButtons.push_back(newButton); //due to custom collision management, we need to keep track of buttons in the menu
	generateNewRollMenuCollision();
	return newButton;
}

std::shared_ptr<MgeButton> MgeRollMenu::getMainButton()
{
	if (!mainButton)
	{
		mainButton = std::make_shared<MgeButton>(IPoint(0, 0), getSize());
		mainButton->setColor(DEFAULT_FRAME_COLOR);
		mainButton->setMouseOverButtonColor(MOUSE_OVER_FRAME_COLOR);
		mainButton->setButtonTextColors(DEFAULT_TEXT_COLOR, MOUSE_OVER_TEXT_COLOR);
		mainButton->setOnLMBClick([this]() { onRollMenuLMBClick(); });
		mainButton->setOnCursorOver([this]() {onCursorEnterCall(); }); //used for auto opening - if it is set
		mainButton->addTextToFrame(mainButtonText, 14);
		mgeGui::addWidgetToGui(getSelfPtr(), mainButton);
	}
	return mainButton;
}

void MgeRollMenu::setAutoOpen(bool newAutoOpen) noexcept
{
	autoOpen = newAutoOpen;
}

void MgeRollMenu::setAutoClose(bool newAutoClose) noexcept
{
	autoClose = newAutoClose;
}

void MgeRollMenu::layout() noexcept
{
	try
	{
		MgeFrame::layout();
		generateNewRollMenuCollision();
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

void MgeRollMenu::setIsVisible(bool visible) noexcept
{
	MgeWidget::setIsVisible(visible);
	_ASSERT(mainButton && rollMenuCollisionFrame);
	if (mainButton)
		mainButton->setIsVisible(visible);
	if (rollMenuCollisionFrame)
		rollMenuCollisionFrame->setIsVisible(visible);
	for (auto& button : rollButtons)
	{
		_ASSERT(button.lock());
		if (auto but = button.lock())
			but->setIsVisible(visible);
	}
}

void MgeRollMenu::onCursorLeaveCall() noexcept
{
	if (autoClose)
		openOrCloseMenu(false/* = close menu*/);
	MgeFrame::onCursorLeaveCall();
}

void MgeRollMenu::onCursorEnterCall() noexcept
{
	if (autoOpen)
		openOrCloseMenu(true/* = open menu*/);
	MgeFrame::onCursorEnterCall();
}

void MgeRollMenu::openOrCloseMenu(bool open) noexcept
{
	isRolled = open;
	for (auto& child : getChildren())
	{
		if (!child)
		{
			_ASSERT(false); //wrong ptr management
			continue;
		}

		if (auto button = std::dynamic_pointer_cast<MgeButton>(child))
		{
			if (button != mainButton)
				button->setIsVisible(open);
		}
	}

	if (rollMenuCollisionFrame)
	{
		for (auto& col : rollMenuCollisionFrame->editCollision())
			col.setEnabled(open);
	}
}

void MgeRollMenu::generateNewRollMenuCollision()
{
	IPoint leftUpCorner = getAbsolutePosition();
	IPoint rightUpCorner = leftUpCorner + IPoint(getSize().width, getSize().height * 2);

	for (const auto& child : rollButtons)
	{
		if (auto rollMenuButton = child.lock())
		{
			auto childPos = rollMenuButton->getAbsolutePosition();
			auto childSize = rollMenuButton->getSize();

			if (childPos.x < leftUpCorner.x)
				leftUpCorner.x = childPos.x;
			else if(childPos.x + childSize.width > rightUpCorner.x)
				rightUpCorner.x = childPos.x + childSize.width;

			if (childPos.y < leftUpCorner.y)
				leftUpCorner.y = childPos.y;
			else if (childPos.y + childSize.height > rightUpCorner.y)
				rightUpCorner.y = childPos.y + childSize.height;
		}
		else
		{
			_ASSERT(false); //wrong ptr management
		}
	}

	auto newSize = mgeType::Size<int>(rightUpCorner.x - leftUpCorner.x, rightUpCorner.y - leftUpCorner.y);

	if (!rollMenuCollisionFrame)
	{ //invisible frame for non-blocking collision up to buttons
		rollMenuCollisionFrame = std::make_shared<MgeFrame>(IPoint(), mgeType::Size<int>());
		rollMenuCollisionFrame->setColor(DEFAULT_FRAME_COLOR);
		rollMenuCollisionFrame->setOnCursorOver
			(
				[this]() noexcept { onCursorEnterCall(); },
				[this]() noexcept { onCursorLeaveCall(); }
			);
		mgeGui::addWidgetToGui(getSelfPtr(), rollMenuCollisionFrame);
	}
	else
		rollMenuCollisionFrame->editCollision().clear();
	auto newCollision = Trigger<int>
		(false, mgeShape::Rectangle<int>(leftUpCorner, newSize));
	newCollision.setIsBlocking(false);
	rollMenuCollisionFrame->editCollision().push_back(std::move(newCollision));

	//move the collision no-blocking frame to the back of the children,
	// so it can be affected by the mouse events before buttons
	auto& children = editChildren();
	auto it = std::find(children.begin(), children.end(), rollMenuCollisionFrame);
	if (it != children.end() && it != children.end() - 1)
		std::iter_swap(it, children.end() - 1);
}

void MgeRollMenu::onRollMenuLMBClick()
{
	openOrCloseMenu(!isRolled);
}
