#include "MgeGui.h"
#include "MgeFrame.h"
#include "MgeButton.h"
#include "MgeWidget.h"
#include "MgeRollMenu.h"
#include "MgeScreen.h"

#include "MlWrapper.h"
#include "BasicTypes.h"
#include "MgeImage.h"
#include "MgeText.h"
#include "BaseApp.h"

MgeGui::MgeGui()
{
    sharedMlWrapper = ML_wrapper::getGlobalMlWrapper();
}

void MgeGui::TickScreen(MgeScreen& screen, double delta)
{
    if (!sharedMlWrapper)
    {
        _ASSERT(false); //wrong ptr management
        return;
    }

    screen.tickEvent(delta);
    sharedMlWrapper->setDrawToGuiView();
    tickScreenChildren(screen);
}

void MgeGui::tickWidgets(const std::vector<std::shared_ptr<MgeActor>>& widgets)
{
    for (const auto& mgeActor : widgets)
    {
        if (!mgeActor)
        {
            _ASSERT(false); // pointer should never be null
            continue;
        }

        auto widget = std::dynamic_pointer_cast<MgeWidget>(mgeActor);
        if (!widget || !widget->getIsVisible())
            continue;

        if (const auto& frame = std::dynamic_pointer_cast<MgeFrame>(widget))
        {
            if (checkWidgetBlocking(*frame))
                actualBlockWidget = frame; //could be replaced by another widget above this one if multiple widgets are under cursor, but this is expected behavior
        }

        tickWidgets(widget->getChildren());
    }
}

void MgeGui::sendLmbEventToGui(const IPoint& clickPos, bool clicked)
{
    if (clicked && actualBlockWidget)
    {
        if (const auto& button = std::dynamic_pointer_cast<MgeButton>(actualBlockWidget))
			button->onLmbClickCall();
    }
    //else -> release not yet
}

void MgeGui::sendRmbEventToGui(const IPoint& cursorPos, bool clicked)
{
    if (clicked && actualBlockWidget)
    {
        if (const auto& button = std::dynamic_pointer_cast<MgeButton>(actualBlockWidget))
            button->onRmbClickCall();
    }
    //else -> release not yet
}

bool MgeGui::isCursorBlockedByGui() const noexcept
{
    return (actualBlockWidget.use_count() != 0);
}

void MgeGui::tickScreenChildren(const MgeScreen& screen)
{
    const std::shared_ptr<MgeWidget> lastBlockWidget = actualBlockWidget;
	actualBlockWidget.reset(); //will be set again if any widget is under cursor
    tickWidgets(screen.getChildren());

    if (lastBlockWidget != actualBlockWidget && lastBlockWidget)
	{ //if lastBlockWidget is not null and is different from actualBlockWidget, it means cursor left the last widget
        if (const auto& releasedFrame = std::dynamic_pointer_cast<MgeFrame>(lastBlockWidget))
            releasedFrame->setUnderMouseCursor(false);
    }

    if (actualBlockWidget)
	{ //if actualBlockWidget is not null, it means cursor is over the widget
        if (const auto& frame = std::dynamic_pointer_cast<MgeFrame>(actualBlockWidget))
            frame->setUnderMouseCursor(true);
    }
}

bool MgeGui::checkWidgetBlocking(const MgeFrame& frame) const noexcept
{
    if (frame.getIsVisible())
    {
        auto& cursorPos = sharedMlWrapper->getCursorGuiPosition();
        for (const auto& col : frame.getCollision())
        {
            if (col.getEnabled() && col.getBlocking() && col.isPointInside(cursorPos))
                return true;
        }
    }
    return false;
}
