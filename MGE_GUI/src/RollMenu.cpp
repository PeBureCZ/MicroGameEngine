#include "RollMenu.h"

RollMenu::RollMenu(IPoint newPosition, mgeType::Size<int> newSize, std::string text)
	: Frame(newPosition, newSize)
{
	mainButtonText = text;
}

std::shared_ptr<Button> RollMenu::addRollButton(std::string butText, Callback onLMBClickFunc, Callback onRMBClickFunc)
{
	getMainButton(); //generate main but if not exist

	size_t relativeYpos_pxls = getSize().height * (rollButtons.size() + 1);
	auto newButton = std::make_shared<Button>(IPoint(0, (int)relativeYpos_pxls), getSize());
	newButton->setIsVisible(false);
	newButton->setOnLMBClick(onLMBClickFunc);
	newButton->setOnRMBClick(onRMBClickFunc);
	
	auto newText = ML_wrapper::getGlobalGameWrapper()->createText(butText, 14);
	if (newText.has_value())
		newButton->addTextToFrame(newText.value());

	guiAdd::addWidgetToGui(getSelfPtr(), std::dynamic_pointer_cast<Widget>(newButton));
	rollButtons.push_back(newButton); //due to custom collision management, we need to keep track of buttons in the menu
	generateNewRollMenuCollision();
	return newButton;
}

std::shared_ptr<Button> RollMenu::getMainButton()
{
	if (!mainButton)
	{
		mainButton = std::make_shared<Button>(IPoint(0, 0), getSize());
		mainButton->setColor(DEFAULT_FRAME_COLOR);
		mainButton->setMouseOverButtonColor(MOUSE_OVER_FRAME_COLOR);
		mainButton->setButtonTextColors(DEFAULT_TEXT_COLOR, MOUSE_OVER_TEXT_COLOR);
		mainButton->setOnLMBClick([this]() { onRollMenuLMBClick(); });
		mainButton->setOnCursorOver([this]() {onCursorEnterCall(); }); //used for auto opening - if it is set
		auto newText = ML_wrapper::getGlobalGameWrapper()->createText(mainButtonText, 14);
		if (newText.has_value())
			mainButton->addTextToFrame(newText.value());
		guiAdd::addWidgetToGui(getSelfPtr(), std::dynamic_pointer_cast<Widget>(mainButton));
	}
	return mainButton;
}

void RollMenu::setAutoOpen(bool newAutoOpen) noexcept
{
	autoOpen = newAutoOpen;
}

void RollMenu::setAutoClose(bool newAutoClose) noexcept
{
	autoClose = newAutoClose;
}

void RollMenu::layout() noexcept
{
	try
	{
		Frame::layout();
		generateNewRollMenuCollision();
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

void RollMenu::onCursorLeaveCall() noexcept
{
	if (autoClose)
		openOrCloseMenu(false/* = close menu*/);
	Frame::onCursorLeaveCall();
}

void RollMenu::onCursorEnterCall() noexcept
{
	if (autoOpen)
		openOrCloseMenu(true/* = open menu*/);
	Frame::onCursorEnterCall();
}

void RollMenu::openOrCloseMenu(bool open) noexcept
{
	isRolled = open;
	for (auto& button : getChildren())
	{
		if (!button)
		{
			_ASSERT(false); //wrong ptr management
			continue;
		}
		if (button  != mainButton)
			button->setIsVisible(open);
	}

	if (rollMenuCollisionFrame)
	{
		for (auto& col : rollMenuCollisionFrame->editCollision())
			col.setEnabled(open);
	}
}

void RollMenu::generateNewRollMenuCollision()
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
		rollMenuCollisionFrame = std::make_shared<Frame>(IPoint(), mgeType::Size<int>());
		rollMenuCollisionFrame->setColor(mgeType::Color_RGBA(0,0,0,0)); 
		rollMenuCollisionFrame->setOnCursorOver
			(
				[this]() noexcept { onCursorEnterCall(); },
				[this]() noexcept { onCursorLeaveCall(); }
			);
		guiAdd::addWidgetToGui(getSelfPtr(), std::dynamic_pointer_cast<Widget>(rollMenuCollisionFrame));
	
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

void RollMenu::onRollMenuLMBClick()
{
	openOrCloseMenu(!isRolled);
}
