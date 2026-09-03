#pragma once

#include <memory> 
#include <vector>

#include "BasicShapes.h"
#include "BasicTypes.h"

#include "GraphicDependencies.h"

#include "MgeGui.h"
#include "GlobalEvents.h"
#include "GuiDependencies.h"

namespace ML_wrapper
{
	class MlWrapper;
}

class MgeScreen;
class MgeWidget;
struct AppTermination;
struct MouseClick;
struct WheelScroll;

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
	void setActualScreen(std::shared_ptr<MgeScreen> newScreen);
	void addScreenToGui(std::shared_ptr<MgeScreen> screen);
	[[nodiscard]] std::shared_ptr<MgeScreen> getActualScreen() const noexcept;

	/**could be managed in derived*/
	virtual void onAppTick(double deltaTime);
	/**could be managed in derived*/
	virtual void afterAppTick();

	template <typename Event>
	void sendEvent(Event&& ev)
	{
		getEventSystem().pushEvent(std::move(ev));
	}

	template <typename Event, typename Function>
	void bindEvent(Function&& fce)
	{
		eventsLifeTimeObserver.addToken<Event>(std::forward<Function>(fce));
	}

	MgeGui gui;

private:
	bool gameIsRunning = false;
	std::shared_ptr<ML_wrapper::MlWrapper> sharedMlWrapper;
	std::shared_ptr<MgeScreen> actualScreen;
	ObserverTokens eventsLifeTimeObserver;

	void processMouseClickEvent(const MouseClick& clickEvent);
	void processWheelEvent(const WheelScroll& wheelEvent);
	void processTerminationEvent(const AppTermination& exitEvent);
	void processResizeEvent();
	void tickApplication(double deltaTime);
};
