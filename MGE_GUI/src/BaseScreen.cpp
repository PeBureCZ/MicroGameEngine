#include "BaseScreen.h"

BaseScreen::BaseScreen(SCREEN_EVENTS eventDeque)
	: Widget(IPoint(), mgeType::Size<int>())
{
	_ASSERT(eventDeque);
	mlGameWrapper = ML_wrapper::getGlobalGameWrapper();
	screenEvents = eventDeque;
}

std::shared_ptr<ML_wrapper::MlGameWrapper> BaseScreen::getGameWrapper()
{
	if (auto gameWrapper = mlGameWrapper.lock())
		return gameWrapper;
	_ASSERT(false);
	return std::make_shared<ML_wrapper::MlGameWrapper>();
}

void BaseScreen::lmbPressEvent(IPoint pos)
{
	//may be used in derived class
}

void BaseScreen::rmbPressEvent(IPoint pos)
{
	//may be used in derived class
}

void BaseScreen::lmbReleaseEvent(IPoint pos)
{
	//may be used in derived class
}

void BaseScreen::rmbReleaseEvent(IPoint pos)
{
	//may be used in derived class
}

void BaseScreen::middleMousePressEvent(IPoint pos)
{
	//may be used in derived class
}

void BaseScreen::middleMouseReleaseEvent(IPoint pos)
{
	//may be used in derived class
}

void BaseScreen::wheelScrollEventCall(WheelScroll scroll)
{
	//may be used in derived class

}

FPoint BaseScreen::getRelativeFromCursor()
{
	return getGameWrapper()->getCursorWorldPosition().asFloat();
}

void BaseScreen::addEvent(ScreenEvent event)
{
	_ASSERT(screenEvents);
	if (screenEvents)
		screenEvents->emplace_back(event);
}

void BaseScreen::tickEvent(double delta)
{
	//may be used in derived class
}

