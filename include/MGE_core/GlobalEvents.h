#pragma once

#include "EventSystem.h"

enum MgeEventType : uint64_t
{
    MgeEventType_None = 0,
    MgeScreenEvent,
    MgeMouseClick,
    MgeWheel,
    ExitApp,
    ResizeWindow,
    //add new event types here...

    STATIC_CHECKER,
    lastMgeReserved = 20000
    //user-defined events below...
};

static_assert(STATIC_CHECKER < lastMgeReserved);

#define MGE_EVENT_TYPE static constexpr uint64_t mgeEventType

//EXAMPLE:
//struct TemplatedEvent
//{
//    MGE_EVENT_TYPE = MgeEventType::lastMgeReserved + 1; // Example event type
//    std::string data = "test";
//};

struct TerminateApp
{
	MGE_EVENT_TYPE = MgeEventType::ExitApp;
};

struct resizeWindowEvent
{
    MGE_EVENT_TYPE = MgeEventType::ResizeWindow;
};


