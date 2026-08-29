#pragma once

#include <memory>

#include "MgeButton.h"

#include "Eventsystem.h"
#include "BasicTypes.h"

namespace ML_wrapper
{
	class MlWrapper;
}

class MgeScreen : public MgeWidget
{
public:
	MgeScreen();
	virtual ~MgeScreen() {};

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
	std::shared_ptr<ML_wrapper::MlWrapper> getMlWrapper();

private:
	std::weak_ptr<ML_wrapper::MlWrapper> mlWrapper;
};

