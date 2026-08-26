#include "BaseApp.h"

#include "MlWrapper.h"
#include "ScreenEvent.h"
#include "EventTable.h"
#include "BaseScreen.h"
#include "GlobalFunctions.h"
#include "EventSystem.h"

BaseApp::BaseApp()
{
    getMlWrapper(); //run game
    gameIsRunning = true;
}

void BaseApp::onAppTick(double deltaTime)
{
    //could be used in derived classes
}

void BaseApp::afterAppTick()
{
    //could be used in derived classes
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

    processMlEvents();

    auto& screenEvents = gui.getScreenEventQue();
    _ASSERT(screenEvents);
    if (screenEvents && screenEvents->size() > 0)
        callScreenEvents(screenEvents);

	sharedMlWrapper->displayActualFrame(); //draw everything to main window and display it

    afterAppTick();
}

void BaseApp::processMlEvents()
{
    auto& events = sharedMlWrapper->getMlEvents();
    for (const auto& eventFromMl : events)
    {
        if (std::holds_alternative<MouseClick>(eventFromMl))
        {
            MouseClick click = std::get<MouseClick>(eventFromMl);
            auto& cursorPos = sharedMlWrapper->getCursorGuiPosition();
            if (click.getType() == MlEventTypeEnum::MouseLeftClick)
            {
                gui.sendLmbEventToGui(cursorPos, click.pressed); //main gui event processing (buttons etc.)
                if (actualScreen && !gui.isCursorBlockedByGui()) //process to actual screen (for example, for game world objects)
                    (click.pressed) ? actualScreen->lmbPressEvent(cursorPos) : actualScreen->lmbReleaseEvent(cursorPos);
            }
            else if (click.getType() == MlEventTypeEnum::MouseRightClick)
            {
                gui.sendRmbEventToGui(cursorPos, click.pressed); //main gui event processing (buttons etc.)
                if (actualScreen && !gui.isCursorBlockedByGui()) //process to actual screen (for example, for game world objects)
                    (click.pressed) ? actualScreen->rmbPressEvent(cursorPos) : actualScreen->rmbReleaseEvent(cursorPos);
            }
            else if (click.getType() == MlEventTypeEnum::WheelMiddleButton)
            {
                if (actualScreen && !gui.isCursorBlockedByGui()) //process to actual screen (for example, for game world objects)
                    (click.pressed) ? actualScreen->middleMousePressEvent(cursorPos) : actualScreen->middleMouseReleaseEvent(cursorPos);
            }
        }
        else if (std::holds_alternative<WheelScroll>(eventFromMl))
        {
            WheelScroll scroll = std::get<WheelScroll>(eventFromMl);
            if (actualScreen && !gui.isCursorBlockedByGui())
                actualScreen->wheelScrollEventCall(scroll);
        }
        else if (std::holds_alternative<AppTermination>(eventFromMl))
        {
            AppTermination click = std::get<AppTermination>(eventFromMl);
            gameIsRunning = false;
        }
    }
    sharedMlWrapper->clearEvents();
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
            auto event = getEventSystem().pullEvent();
            getEventSystem().publishEvent(event);
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

void BaseApp::callScreenEvents(SCREEN_EVENTS& events)
{
	while (!events->empty()) //need to be overriden in game class to process game specific events
        events->pop_front();
}

void BaseApp::setActualScreen(std::shared_ptr<BaseScreen> newScreen)
{
    _ASSERT(newScreen);
	actualScreen = newScreen;
}