#pragma once

#include <memory>

#include "Button.h"

#include "ScreenEvent.h"
#include "BasicTypes.h"

namespace ML_wrapper
{
	class MlWrapper;
}

class BaseScreen : public Widget
{
public:
	BaseScreen(SCREEN_EVENTS eventDeque);
	virtual ~BaseScreen() {};

	virtual void tickEvent(double delta);
	virtual void lmbPressEvent(IPoint pos);
	virtual void rmbPressEvent(IPoint pos);
	virtual void lmbReleaseEvent(IPoint pos);
	virtual void rmbReleaseEvent(IPoint pos);
	virtual void middleMousePressEvent(IPoint pos);
	virtual void middleMouseReleaseEvent(IPoint pos);
	virtual void wheelScrollEventCall(WheelScroll scroll);

	FPoint getRelativeFromCursor();

protected:
	std::weak_ptr<ML_wrapper::MlWrapper> mlWrapper;

	SCREEN_EVENTS screenEvents;
	std::shared_ptr<ML_wrapper::MlWrapper> getMlWrapper();

	void addEvent(ScreenEvent event);

private:
};

