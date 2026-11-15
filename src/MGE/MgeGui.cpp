#include "MgeGui.h"

#include <tuple>

#include "MgeFrame.h"
#include "MgeButton.h"
#include "MgeWidget.h"
#include "MgeRollMenu.h"
#include "MgeScreen.h"
#include "MgeWindow.h"
#include "MgeTextFrame.h"

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

void MgeGui::tickScreenChildren(const MgeScreen& screen)
{
    const mge::Widget lastBlockWidget = m_actualBlockWidget.m_widget;
    m_actualBlockWidget.reset();
    if (!m_focusedWidget.expired())
    {
        auto focusedWidget = m_focusedWidget.lock();
        if (!focusedWidget->getIsVisible())
        {
            focusedWidget->onFocusLost();
            m_focusedWidget.reset();
        }
    }
    tickWidgets(screen.getChildren());

    if (lastBlockWidget != m_actualBlockWidget.m_widget && lastBlockWidget)
    { //if lastBlockWidget is not null and is different from m_actualBlockWidget, it means cursor left the last widget
        if (const auto& releasedFrame = std::dynamic_pointer_cast<MgeFrame>(lastBlockWidget))
            releasedFrame->setUnderMouseCursor(false);
    }

    if (m_actualBlockWidget.m_widget)
    { //if actualBlockWidget is not null, it means cursor is over the widget
        if (m_clickEventActive)
        {
            if (const auto& window = std::dynamic_pointer_cast<MgeWindow>(m_actualBlockWidget.m_widget))
            { //move window
                const auto& actualCursorPos = ML_wrapper::getGlobalMlWrapper()->getCursorWorldPosition();
                auto dif = actualCursorPos - m_lastGuiMovePos;
                window->setAbsolutePosition(window->getAbsolutePosition() + dif);
                m_lastGuiMovePos = actualCursorPos;
                window->layout();
            }
        }

        if (const auto& frame = std::dynamic_pointer_cast<MgeFrame>(m_actualBlockWidget.m_widget))
            frame->setUnderMouseCursor(true);
    }
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

        const auto& frame = std::dynamic_pointer_cast<MgeFrame>(widget);
        if (frame && checkWidgetBlocking(*frame))
        {
            if (auto graphic = frame->getGraphicComponent())
            {
                size_t frameLayer = frame->getLayer();
                auto frameZPosition_opt = graphic->getZPosition(GraphicType::BASIC_GRAPHIC_INDEX);

                if (frameZPosition_opt.has_value())
                {
                    //same order as drawing: layer, draw group (window), z-position; equal -> later (child) widget wins
                    const auto candidate = std::make_tuple(frameLayer, frame->getDrawGroup(), frameZPosition_opt.value());
                    const auto actual = std::make_tuple(m_actualBlockWidget.m_layer, m_actualBlockWidget.m_drawGroup, m_actualBlockWidget.m_zPosition);
                    if (candidate >= actual)
                    {
                        m_actualBlockWidget.m_layer = frameLayer;
                        m_actualBlockWidget.m_drawGroup = frame->getDrawGroup();
                        m_actualBlockWidget.m_zPosition = frameZPosition_opt.value();
                        m_actualBlockWidget.m_widget = frame;
                    }
                }
            }
        }

        tickWidgets(widget->getChildren());
    }
}

void MgeGui::sendLmbEventToGui(const IPoint& clickPos, bool clicked)
{
    m_clickEventActive = clicked;

    if (clicked)
        setFocusedWidget(m_actualBlockWidget.m_widget); //non-focusable or empty click -> clears focus

    if (clicked && m_actualBlockWidget.m_widget)
    {
        m_lastGuiMovePos = clickPos;
        bringWindowToFront(m_actualBlockWidget.m_widget);
        if (const auto& button = std::dynamic_pointer_cast<MgeButton>(m_actualBlockWidget.m_widget))
            button->onLmbClickCall();
    }
    //else -> release not yet
}

void MgeGui::sendRmbEventToGui(const IPoint& cursorPos, bool clicked)
{
    if (clicked && m_actualBlockWidget.m_widget)
    {
        if (const auto& button = std::dynamic_pointer_cast<MgeButton>(m_actualBlockWidget.m_widget))
            button->onRmbClickCall();
    }
    //else -> release not yet
}

bool MgeGui::isCursorBlockedByGui() const noexcept
{
    return (m_actualBlockWidget.m_widget.use_count() != 0);
}

void MgeGui::setFocusedWidget(const mge::Widget& widget)
{
    const mge::Widget newFocus = (widget && widget->getIsFocusable() && widget->getIsVisible()) ? widget : nullptr;
    const mge::Widget oldFocus = m_focusedWidget.lock();
    if (oldFocus == newFocus)
        return; //no change -> no double onFocusGet / double bindings

    if (oldFocus)
        oldFocus->onFocusLost();
    m_focusedWidget = newFocus;
    if (newFocus)
        newFocus->onFocusGet();
}

void MgeGui::releaseFocus(const mge::Widget& widget)
{
    auto actualWidget = m_focusedWidget.lock();
    if (actualWidget && widget == actualWidget)
    {
        actualWidget->onFocusLost();
        m_focusedWidget.reset();
    }
}

mge::Widget MgeGui::getFocusedWidget() const noexcept
{
    return m_focusedWidget.lock();
}

void MgeGui::bringWindowToFront(const mge::Widget& widget) noexcept
{
    try
    {
        //the outermost window wins, so a click into a nested window raises the whole top-level window too
        std::shared_ptr<MgeActor> actor = widget;
        std::shared_ptr<MgeWindow> topWindow;
        while (actor)
        {
            if (auto window = std::dynamic_pointer_cast<MgeWindow>(actor))
                topWindow = std::move(window);
            actor = actor->getParent();
        }

        if (topWindow)
            topWindow->bringToFront();
    }
    catch (...)
    {
        _ASSERT(false); //unexpected exception
    }
}

bool MgeGui::checkWidgetBlocking(const MgeFrame& frame) const noexcept
{
    if (frame.getIsVisible())
    {
        auto& cursorPos = sharedMlWrapper->getCursorGuiPosition();
        for (const auto& col : frame.getCollisions())
        {
            if (col.getEnabled() && col.getBlocking() && col.isPointInside(cursorPos))
                return true;
        }
    }
    return false;
}
