#include "MgeGui.h"
#include "Frame.h"
#include "Button.h"
#include "Widget.h"
#include "RollMenu.h"
#include "BaseScreen.h"

#include "MlGameWrapper.h"
#include "BasicTypes.h"
#include "MlImage.h"
#include "MlText.h"
#include "BaseApp.h"

MgeGui::MgeGui()
{
    screenEvents = std::make_shared<std::deque<ScreenEvent>>();
    sharedMlGameWrapper = ML_wrapper::getGlobalGameWrapper();
}

void MgeGui::TickScreen(BaseScreen& screen, double delta)
{
    if (!sharedMlGameWrapper)
    {
        _ASSERT(false); //wrong ptr management
        return;
    }

    screen.tickEvent(delta);
    sharedMlGameWrapper->setDrawToGuiView();
    tickScreenChildren(screen);
}

void MgeGui::tickWidgets(const std::vector<std::shared_ptr<MgeActor>>& widgets)
{
    auto tickFrame = [&](const std::shared_ptr<Frame>& frame)
        {
            if (!frame)
            {
                _ASSERT(false); // pointer should never be null
                return;
            }

            drawFrameObjects(*frame);
			if (checkWidgetBlocking(*frame))
				actualBlockWidget = frame; //could be replaced by another widget above this one if multiple widgets are under cursor, but this is expected behavior
        };

    for (const auto& mgeActor : widgets)
    {
        if (!mgeActor)
        {
            _ASSERT(false); // pointer should never be null
            continue;
        }

        auto widget = std::dynamic_pointer_cast<Widget>(mgeActor);
        if (!widget || !widget->getIsVisible())
            continue;

        if (const auto& frame = std::dynamic_pointer_cast<Frame>(widget))
            tickFrame(frame);

        tickWidgets(widget->getChildren());
    }
}

SCREEN_EVENTS& MgeGui::getScreenEventQue()
{
    return screenEvents;
}

void MgeGui::sendLmbEventToGui(const IPoint& clickPos, bool clicked)
{
    if (clicked && actualBlockWidget)
    {
        if (const auto& button = std::dynamic_pointer_cast<Button>(actualBlockWidget))
			button->onLmbClickCall();
    }
    //else -> release not yet
}

void MgeGui::sendRmbEventToGui(const IPoint& cursorPos, bool clicked)
{
    if (clicked && actualBlockWidget)
    {
        if (const auto& button = std::dynamic_pointer_cast<Button>(actualBlockWidget))
            button->onRmbClickCall();
    }
    //else -> release not yet
}

bool MgeGui::isCursorBlockedByGui() const noexcept
{
    return (actualBlockWidget.use_count() != 0);
}

void MgeGui::tickScreenChildren(const BaseScreen& screen)
{
    const std::shared_ptr<Widget> lastBlockWidget = actualBlockWidget;
	actualBlockWidget.reset(); //will be set again if any widget is under cursor
    tickWidgets(screen.getChildren());

    if (lastBlockWidget != actualBlockWidget && lastBlockWidget)
	{ //if lastBlockWidget is not null and is different from actualBlockWidget, it means cursor left the last widget
        if (const auto& releasedFrame = std::dynamic_pointer_cast<Frame>(lastBlockWidget))
            releasedFrame->setUnderMouseCursor(false);
    }

    if (actualBlockWidget)
	{ //if actualBlockWidget is not null, it means cursor is over the widget
        if (const auto& frame = std::dynamic_pointer_cast<Frame>(actualBlockWidget))
            frame->setUnderMouseCursor(true);
    }
}

void MgeGui::drawFrameObjects(const Frame& frame)
{
    const auto& frameObject = frame.getFrameObject();

    if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
    {
        auto& image = std::get<std::shared_ptr<MlImage>>(frameObject);
        sharedMlGameWrapper->drawGuiSprite(image->getSpriteID());
    }
    else if (std::holds_alternative<DrawableObject<float>>(frameObject))
    {
        auto vertices_opt = frame.getVertices();
        if (vertices_opt.has_value() && vertices_opt.value())
            sharedMlGameWrapper->drawGuiVertices(vertices_opt.value());
    }
    else
    {
    } // nothing to draw

    for (const auto& text : frame.getTextsFromFrame())
        sharedMlGameWrapper->drawText(text.second);
}

bool MgeGui::checkWidgetBlocking(const Frame& frame) const noexcept
{
    if (frame.getIsVisible())
    {
        auto& cursorPos = sharedMlGameWrapper->getCursorGuiPosition();
        for (const auto& col : frame.getCollision())
        {
            if (col.getEnabled() && col.getBlocking() && col.isPointInside(cursorPos))
                return true;
        }
    }
    return false;
}
