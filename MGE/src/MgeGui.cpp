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

    auto& cursorPos = sharedMlGameWrapper->getCursorGuiPosition();

    //widget collisions
    auto blockedCollision = sendGuiCollision(screen.getChildren(), cursorPos);
    if (actualBlockWidget && !blockedCollision)
        actualBlockWidget = nullptr;

    tickWidgets(screen.getChildren());
}

void MgeGui::tickWidgets(const std::vector<std::shared_ptr<MgeActor>>& widgets)
{
    auto tickFrame = [&](const std::shared_ptr<Frame> frame)
        {
            if (!frame)
            {
                _ASSERT(false); // pointer should never be null
                return;
            }
            const auto& frameObject = frame->getFrameObject();

            if (std::holds_alternative<std::shared_ptr<MlImage>>(frameObject))
            {
                auto& image = std::get<std::shared_ptr<MlImage>>(frameObject);
                sharedMlGameWrapper->drawGuiSprite(image->getSpriteID());
            }
            else if (std::holds_alternative<DrawableObject<float>>(frameObject))
            {
                auto vertices_opt = frame->getVertices();
                if (vertices_opt.has_value() && vertices_opt.value())
                    sharedMlGameWrapper->drawGuiVertices(vertices_opt.value());
            }
            else
            {} // nothing to draw

            for (const auto& text : frame->getTextsFromFrame())
                sharedMlGameWrapper->drawText(text.second);
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

bool MgeGui::sendGuiCollision
    (const std::vector<std::shared_ptr<MgeActor>>& guiWidgets, const IPoint& cursorPos)
{
    if (guiWidgets.empty())
        return false;

    bool blocked = false;

    //need to check collisions in the oposite order (the top view widget is in last vector position)  
    for (int i = static_cast<int>(guiWidgets.size() - 1); i >= 0; --i)
    {
        const auto& mgeActor = guiWidgets[i];
        if (!mgeActor)
        {
            _ASSERT(false); // pointer should never be null
            continue;
        }

        auto widget = std::dynamic_pointer_cast<Widget>(mgeActor);
        if (!widget || !widget->getIsVisible())
            continue;

        bool collided = false;
        if (const auto& frame = std::dynamic_pointer_cast<Frame>(widget))
        { //ALL FRAME DERIVED COLLISION
            for (const auto& col : frame->getCollision())
            {
                bool enabledCol = col.getEnabled();
                collided = enabledCol && col.isPointInside(cursorPos);
                if (collided)
                {
                    bool isBlockingCol = col.getBlocking();
                    if (!frame->isUnderCursor())
                    {
                        frame->setUnderMouseCursor(true);
                        if (widget != actualBlockWidget && isBlockingCol)
                        {
                            if (const auto& releasedFrame = std::dynamic_pointer_cast<Frame>(actualBlockWidget))
                                releasedFrame->setUnderMouseCursor(false);
                            actualBlockWidget = widget;
                        }
                    }
                    if (isBlockingCol)
                        return true;
                }
                
            }
            if (!collided && frame->isUnderCursor())
                frame->setUnderMouseCursor(false);
        }

        if (widget->getChildren().size() > 0)
        {
            blocked = sendGuiCollision(widget->getChildren(), cursorPos);
            if (blocked)
                return true;
        }
    }

    return blocked;
}



