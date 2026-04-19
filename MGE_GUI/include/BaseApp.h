#pragma once

#include <memory> 
#include <vector>

#include "BasicShapes.h"
#include "BasicTypes.h"

#include "GraphicDependencies.h"

#include "TsmGui.h"

#include "ScreenEvent.h"
#include "GuiDependencies.h"

namespace ML_wrapper
{
	class MlGameWrapper;
}

class BaseScreen;
class Widget;

class BaseApp
{
public:
	BaseApp();

	void tickApplication(double deltaTime);
	bool isRunning() const;

	void terminateApplication() noexcept;

	std::shared_ptr< ML_wrapper::MlGameWrapper> getMlGameWrapper();

	virtual ~BaseApp();

protected:
	virtual void callScreenEvents(SCREEN_EVENTS& events);
	void setActualScreen(std::shared_ptr<BaseScreen> newScreen);

	TsmGui gui;

private:
	bool gameIsRunning = false;
	std::shared_ptr<ML_wrapper::MlGameWrapper> sharedMlGameWrapper;
	std::shared_ptr<BaseScreen> actualScreen;

	void processMlEvents();
};

