#pragma once

#include <memory> 
#include <vector>

#include "BasicShapes.h"
#include "BasicTypes.h"

#include "GraphicDependencies.h"

#include "MgeGui.h"

#include "ScreenEvent.h"
#include "GuiDependencies.h"

namespace ML_wrapper
{
	class MlWrapper;
}

class BaseScreen;
class Widget;

class BaseApp
{
public:
	BaseApp();
	
	bool isRunning() const;

	void runApp();
	void terminateApplication() noexcept;

	std::shared_ptr< ML_wrapper::MlWrapper> getMlWrapper();

	virtual ~BaseApp();

protected:
	virtual void callScreenEvents(SCREEN_EVENTS& events);
	void setActualScreen(std::shared_ptr<BaseScreen> newScreen);

	/**could be managed in derived*/
	virtual void onAppTick(double deltaTime);
	/**could be managed in derived*/
	virtual void afterAppTick();

	MgeGui gui;

private:
	bool gameIsRunning = false;
	std::shared_ptr<ML_wrapper::MlWrapper> sharedMlWrapper;
	std::shared_ptr<BaseScreen> actualScreen;

	void processMlEvents();
	void tickApplication(double deltaTime);
};

