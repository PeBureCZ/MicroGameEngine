#include "MgeScreen.h"

#include "EventSystem.h"

MgeScreen::MgeScreen()
	: MgeWidget(IPoint(), mgeType::Size<int>())
{
	mlWrapper = ML_wrapper::getGlobalMlWrapper();
	setSize(getMlWrapper()->getScreenSize());
}

std::shared_ptr<ML_wrapper::MlWrapper> MgeScreen::getMlWrapper()
{
	if (auto gameWrapper = mlWrapper.lock())
		return gameWrapper;
	_ASSERT(false);
	return std::make_shared<ML_wrapper::MlWrapper>();
}

void MgeScreen::lmbPressEvent(IPoint pos)
{
	//may be used in derived class
}

void MgeScreen::rmbPressEvent(IPoint pos)
{
	//may be used in derived class
}

void MgeScreen::lmbReleaseEvent(IPoint pos)
{
	//may be used in derived class
}

void MgeScreen::rmbReleaseEvent(IPoint pos)
{
	//may be used in derived class
}

void MgeScreen::middleMousePressEvent(IPoint pos)
{
	//may be used in derived class
}

void MgeScreen::middleMouseReleaseEvent(IPoint pos)
{
	//may be used in derived class
}

void MgeScreen::wheelScrollEventCall(WheelScroll scroll)
{
	//may be used in derived class

}

FPoint MgeScreen::getRelativeFromCursor()
{
	return getMlWrapper()->getCursorWorldPosition();
}

void MgeScreen::tickEvent(double delta)
{
	//may be used in derived class
}

