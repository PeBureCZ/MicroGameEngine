#pragma once

#include <memory>
#include <string>
#include <vector>

#include "BasicShapes.h"
#include "BasicTypes.h"

#include "GraphicDependencies.h"

#include "MgeGui.h"
#include "GlobalEvents.h"

namespace ML_wrapper
{
	class MlWrapper;
}

class MgeScreen;
class MgeWidget;
struct AppTermination;
struct AppCloseRequest;
struct MouseClick;
struct WheelScroll;

class BaseApp
{
public:
	BaseApp(bool fullscreen = false, const std::wstring& appName = std::wstring(ML_wrapper::DEFAULT_WINDOW_TITLE));
	
	bool isRunning() const;

	void runApp();
	void terminateApplication() noexcept;

	std::shared_ptr< ML_wrapper::MlWrapper> getMlWrapper();

	/**shown in the render window title bar*/
	void setAppName(const std::wstring& newAppName);
	[[nodiscard]] std::wstring getAppName() const;

	//render window control (see MlWrapper)
	void lockWindowSize(const ISize& clientSize);
	void unlockWindowSize();
	[[nodiscard]] bool isWindowSizeLocked() const;
	void maximizeWindow();
	void minimizeWindow();
	void restoreWindow();
	[[nodiscard]] bool isWindowMaximized() const;
	[[nodiscard]] bool isWindowMinimized() const;
	void setFullscreen(bool isFullscreen);
	[[nodiscard]] bool isFullscreen() const;

	virtual ~BaseApp();

protected:
	void setActualScreen(std::shared_ptr<MgeScreen> newScreen);
	void addScreenToGui(std::shared_ptr<MgeScreen> screen);
	[[nodiscard]] std::shared_ptr<MgeScreen> getActualScreen() const noexcept;

	/**could be managed in derived*/
	virtual void onAppTick(double deltaTime) {};
	/**could be managed in derived*/
	virtual void afterAppTick() {};
	/**could be managed in derived*/
	virtual void processCloseRequestEvent(const AppCloseRequest& closeRequestEvent) { terminateApplication(); }

	template <typename Event>
	void sendEvent(Event&& ev)
	{
		getEventSystem().pushEvent(std::move(ev));
	}

	template <typename Event, typename Function>
	ObserverTokenId bindEvent(Function&& fce)
	{
		return eventsLifeTimeObserver.addToken<Event>(std::forward<Function>(fce));
	}

	MgeGui& getGui();

private:
	std::unique_ptr<MgeGui> m_gui;
	bool gameIsRunning = false;
	std::shared_ptr<ML_wrapper::MlWrapper> sharedMlWrapper;
	std::shared_ptr<MgeScreen> actualScreen;
	ObserverTokens eventsLifeTimeObserver;

	void processMouseClickEvent(const MouseClick& clickEvent);
	void processWheelEvent(const WheelScroll& wheelEvent);
	void processTerminationEvent(const AppTermination& exitEvent);
	void processResizeEvent();
	void processCloseWindowEvent(const CloseMgeWindowEvent& winEvent);
	void tickApplication(double deltaTime);
};
