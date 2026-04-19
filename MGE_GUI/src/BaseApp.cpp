#include "BaseApp.h"

#include "MlGameWrapper.h"
#include "ScreenEvent.h"
#include "EventTable.h"
#include "BaseScreen.h"

BaseApp::BaseApp()
{
    getMlGameWrapper(); //run game
    gameIsRunning = true;
}

void BaseApp::tickApplication(double deltaTime)
{
    if (!sharedMlGameWrapper)
    {
        _ASSERT(false);
        return;
    }

    sharedMlGameWrapper->clear(); //clear main window to begin new frame drawing

	//DRAWING - APPLICATION LAYER
    sharedMlGameWrapper->drawVertices();
    sharedMlGameWrapper->drawSprites();

    //DRAWING - GUI
    sharedMlGameWrapper->pollMlEvents();

    if (actualScreen)
        gui.TickScreen(*actualScreen, deltaTime);

    processMlEvents();

    auto& screenEvents = gui.getScreenEventQue();
    _ASSERT(screenEvents);
    if (screenEvents && screenEvents->size() > 0)
        callScreenEvents(screenEvents);

	sharedMlGameWrapper->displayActualFrame(); //draw everything to main window and display it
}

void BaseApp::processMlEvents()
{
    auto& events = sharedMlGameWrapper->getMlEvents();
    for (const auto& eventFromMl : events)
    {
        if (std::holds_alternative<MouseClick>(eventFromMl))
        {
            MouseClick click = std::get<MouseClick>(eventFromMl);
            auto& cursorPos = sharedMlGameWrapper->getCursorGuiPosition();
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
    sharedMlGameWrapper->clearEvents();
}

bool BaseApp::isRunning() const
{
    return gameIsRunning;
}

void BaseApp::terminateApplication() noexcept
{
	gameIsRunning = false;
}

std::shared_ptr< ML_wrapper::MlGameWrapper> BaseApp::getMlGameWrapper()
{
    if (!sharedMlGameWrapper)
        sharedMlGameWrapper = ML_wrapper::getGlobalGameWrapper();
    return sharedMlGameWrapper;
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
	actualScreen = newScreen;
}