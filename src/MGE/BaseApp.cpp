#include "BaseApp.h"

#include "MlWrapper.h"
#include "MgeScreen.h"
#include "GlobalFunctions.h"
#include "MlEvent.h"

BaseApp::BaseApp()
{
    getMlWrapper(); //run game
    gameIsRunning = true;
    bindEvent<AppTermination>([this](const AppTermination& exitEvent) { processTerminationEvent(exitEvent); });
    bindEvent<MouseClick>([this](const MouseClick& clickEvent) { processMouseClickEvent(clickEvent); });
    bindEvent<WheelScroll>([this](const WheelScroll& wheelEvent) { processWheelEvent(wheelEvent); });
    bindEvent<TerminateApp>([this](const TerminateApp& wheelEvent) { terminateApplication(); });
    bindEvent<resizeWindowEvent>([this](const resizeWindowEvent& wheelEvent) { processResizeEvent(); });
}

void BaseApp::onAppTick(double deltaTime)
{
    //could be used in derived classes
}

void BaseApp::afterAppTick()
{
    //could be used in derived classes
}

void BaseApp::processMouseClickEvent(const MouseClick& clickEvent)
{
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
    if (actualScreen && !gui.isCursorBlockedByGui())
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
    }
}

void BaseApp::tickApplication(double deltaTime)
{
    onAppTick(deltaTime);

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
        gui.TickScreen(*actualScreen, deltaTime);


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
        auto tick_start = std::chrono::high_resolution_clock::now();
        if (!getEventSystem().isEmpty())
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
            catch(...)
            {

            }
        }

        tickApplication(delta_sec.count());
        auto tick_end = std::chrono::high_resolution_clock::now();
        delta_sec = tick_end - tick_start;
    }
}

void BaseApp::terminateApplication() noexcept
{
	gameIsRunning = false;
}

std::shared_ptr< ML_wrapper::MlWrapper> BaseApp::getMlWrapper()
{
    if (!sharedMlWrapper)
        sharedMlWrapper = ML_wrapper::getGlobalMlWrapper();
    return sharedMlWrapper;
}

BaseApp::~BaseApp()
{

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
