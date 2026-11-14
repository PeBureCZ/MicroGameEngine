#include "BaseApp.h"

#include "MlWrapper.h"
#include "MgeScreen.h"
#include "GlobalFunctions.h"
#include "MlEvent.h"

#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

namespace
{
    constexpr unsigned int MAX_FRAME_RATE = 60;
    constexpr auto FRAME_DURATION = std::chrono::duration<double>(1.0 / MAX_FRAME_RATE);
}

BaseApp::BaseApp(bool fullscreen, const std::wstring& appName)
{
    timeBeginPeriod(1);   // 1 ms timer precision for accurate frame sleeps
    mgeCore::setThisThreadAsMain();
    [[maybe_unused]] bool initialized = ML_wrapper::initializeMlWrapper(fullscreen, appName);
    _ASSERT(initialized);
    sharedMlWrapper = ML_wrapper::getGlobalMlWrapper();
    gameIsRunning = true;
    getGui(); //initialize gui
    bindEvent<AppTermination>([this](const AppTermination& exitEvent) { processTerminationEvent(exitEvent); });
    bindEvent<MouseClick>([this](const MouseClick& clickEvent) { processMouseClickEvent(clickEvent); });
    bindEvent<WheelScroll>([this](const WheelScroll& wheelEvent) { processWheelEvent(wheelEvent); });
    bindEvent<AppCloseRequest>([this](const AppCloseRequest& closeRequestEvent) { processCloseRequestEvent(closeRequestEvent); });
    bindEvent<ResizeWindowEvent>([this](const ResizeWindowEvent& wheelEvent) { processResizeEvent(); });
    bindEvent<CloseMgeWindowEvent>([this](const CloseMgeWindowEvent& winEvent) { processCloseWindowEvent(winEvent); });
    bindEvent<FocusWidgetEvent>([this](const FocusWidgetEvent& e)
        { e.m_focus ? getGui().setFocusedWidget(e.m_widget.lock()) : getGui().releaseFocus(e.m_widget.lock()); });
}

MgeGui& BaseApp::getGui()
{
    MAIN_THREAD_GUARD;
    if (!m_gui)
        m_gui = std::make_unique<MgeGui>();
    return *m_gui;
}

void BaseApp::processMouseClickEvent(const MouseClick& clickEvent)
{
    auto& gui = getGui();
    if (clickEvent.getType() == MlEventTypeEnum::MouseLeftClick)
    {
        gui.sendLmbEventToGui(clickEvent.clickPosition, clickEvent.pressed); //main gui event processing (buttons etc.)
        if (actualScreen && !gui.isCursorBlockedByGui()) //process to actual screen (for example, for game world objects)
            (clickEvent.pressed) ? actualScreen->lmbPressEvent(clickEvent.clickPosition) : actualScreen->lmbReleaseEvent(clickEvent.clickPosition);
    }
    else if (clickEvent.getType() == MlEventTypeEnum::MouseRightClick)
    {
        gui.sendRmbEventToGui(clickEvent.clickPosition, clickEvent.pressed); //main gui event processing (buttons etc.)
        if (actualScreen && !gui.isCursorBlockedByGui()) //process to actual screen (for example, for game world objects)
            (clickEvent.pressed) ? actualScreen->rmbPressEvent(clickEvent.clickPosition) : actualScreen->rmbReleaseEvent(clickEvent.clickPosition);
    }
    else if (clickEvent.getType() == MlEventTypeEnum::WheelMiddleButton)
    {
        if (actualScreen && !gui.isCursorBlockedByGui()) //process to actual screen (for example, for game world objects)
            (clickEvent.pressed) ? actualScreen->middleMousePressEvent(clickEvent.clickPosition) : actualScreen->middleMouseReleaseEvent(clickEvent.clickPosition);
    }
}

void BaseApp::processWheelEvent(const WheelScroll& wheelEvent)
{
    if (actualScreen && !getGui().isCursorBlockedByGui())
        actualScreen->wheelScrollEventCall(wheelEvent);
}

void BaseApp::processTerminationEvent(const AppTermination& exitEvent)
{
    gameIsRunning = false;
}

void BaseApp::processResizeEvent()
{
    if (actualScreen)
    {
        auto mlWrapper = ML_wrapper::getGlobalMlWrapper();
        actualScreen->setSize(mlWrapper->getRenderWindowSize());
        actualScreen->layout();
    }
}

void BaseApp::processCloseWindowEvent(const CloseMgeWindowEvent& winEvent)
{
    if (actualScreen)
    {
        actualScreen->removeChildFromWidget(winEvent.m_widgetId);
    }
}

void BaseApp::tickApplication(double deltaTime_sec)
{
    onAppTick(deltaTime_sec);

    if (!sharedMlWrapper)
    {
        _ASSERT(false);
        return;
    }

    sharedMlWrapper->clear(); //clear main window to begin new frame drawing

	//DRAWING - APPLICATION LAYER
    sharedMlWrapper->drawContent();

    //DRAWING - GUI
    sharedMlWrapper->pollMlEvents();

    if (actualScreen)
        getGui().TickScreen(*actualScreen, deltaTime_sec);

	sharedMlWrapper->displayActualFrame(); //draw everything to main window and display it

    afterAppTick();
}

bool BaseApp::isRunning() const
{
    return gameIsRunning;
}

void BaseApp::runApp()
{
    if (!mgeCore::mainThreadIsSet())
        mgeCore::setThisThreadAsMain();
    _ASSERT(mgeCore::mainThreadIsSet());
    std::chrono::duration<double> delta_sec(0.0);

    while (isRunning())
    {
        int maxLoopRepeat = static_cast<int>(getEventSystem().getEventCount());
        auto tickStart_ns = std::chrono::steady_clock::now();
        while (maxLoopRepeat-- > 0)
        {
            try
            {
                auto event = getEventSystem().pullEvent();
                getEventSystem().publishEvent(event);
            }
#ifdef _DEBUG
            catch (const std::exception& e)
            {
                auto err = e.what();
                _ASSERT(false);
            }
#endif //_DEBUG
            catch (...)
            {

            }
        }

        tickApplication(delta_sec.count());

        std::this_thread::sleep_until(tickStart_ns + std::chrono::duration_cast<std::chrono::steady_clock::duration>(FRAME_DURATION));

        auto tickEnd_ns = std::chrono::steady_clock::now();
        delta_sec = tickEnd_ns - tickStart_ns;
    }
}

void BaseApp::terminateApplication() noexcept
{
	gameIsRunning = false;
}

std::shared_ptr< ML_wrapper::MlWrapper> BaseApp::getMlWrapper()
{
    _ASSERT(sharedMlWrapper);
    if (!sharedMlWrapper)
        sharedMlWrapper = ML_wrapper::getGlobalMlWrapper();
    return sharedMlWrapper;
}

void BaseApp::setAppName(const std::wstring& newAppName)
{
	getMlWrapper()->setWindowTitle(newAppName);
}

std::wstring BaseApp::getAppName() const
{
	_ASSERT(sharedMlWrapper);
	return sharedMlWrapper ? sharedMlWrapper->getWindowTitle() : std::wstring();
}

void BaseApp::lockWindowSize(const ISize& clientSize)
{
	getMlWrapper()->lockWindowSize(clientSize);
}

void BaseApp::unlockWindowSize()
{
	getMlWrapper()->unlockWindowSize();
}

bool BaseApp::isWindowSizeLocked() const
{
	return sharedMlWrapper && sharedMlWrapper->isWindowSizeLocked();
}

void BaseApp::maximizeWindow()
{
	getMlWrapper()->maximizeWindow();
}

void BaseApp::minimizeWindow()
{
	getMlWrapper()->minimizeWindow();
}

void BaseApp::restoreWindow()
{
	getMlWrapper()->restoreWindow();
}

bool BaseApp::isWindowMaximized() const
{
	return sharedMlWrapper && sharedMlWrapper->isWindowMaximized();
}

bool BaseApp::isWindowMinimized() const
{
	return sharedMlWrapper && sharedMlWrapper->isWindowMinimized();
}

void BaseApp::setFullscreen(bool isFullscreen)
{
	getMlWrapper()->setFullscreen(isFullscreen);
}

bool BaseApp::isFullscreen() const
{
	return sharedMlWrapper && sharedMlWrapper->isFullscreen();
}

BaseApp::~BaseApp()
{
    timeEndPeriod(1);     // restore default timer precision
}

void BaseApp::setActualScreen(std::shared_ptr<MgeScreen> newScreen)
{
    _ASSERT(newScreen);
    actualScreen = newScreen;
    if (actualScreen)
    {
        if (!actualScreen->isInitialized())
            addScreenToGui(newScreen);
        auto oldSize = actualScreen->getSize();
        auto newSize = ML_wrapper::getGlobalMlWrapper()->getRenderWindowSize();
        if (oldSize != newSize)
        {
            actualScreen->setSize(newSize);
            actualScreen->layout();
        }
    }
}

void BaseApp::addScreenToGui(std::shared_ptr<MgeScreen> screen)
{
	_ASSERT(screen && !screen->isInitialized()); //initialize only once! (or wrong ptr)
    if (screen && !screen->isInitialized())
    {
        screen->initializeSelf(screen);
		screen->initialize(); //can be overridden
    }
}

std::shared_ptr<MgeScreen> BaseApp::getActualScreen() const noexcept
{
    return actualScreen;
}
