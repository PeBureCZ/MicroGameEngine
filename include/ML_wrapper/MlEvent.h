#pragma once

#include <variant>

#include "GlobalEvents.h"
#include "BasicTypes.h"

constexpr bool PRESSED = true;
constexpr bool RELEASED = false;

enum class MlEventTypeEnum
{
	MouseLeftClick,
	MouseRightClick,
	WheelMiddleButton
};

struct MouseClick
{
	MGE_EVENT_TYPE = MgeEventType::MgeMouseClick;
	IPoint clickPosition;
	bool pressed = false;
	MlEventTypeEnum type;
	MouseClick(int x, int y, bool isPressed, MlEventTypeEnum type)
		:pressed(isPressed), type(type)
	{
		clickPosition.x = x;
		clickPosition.y = y;
	}
	MouseClick(IPoint pos, bool isPressed, MlEventTypeEnum type)
		:pressed(isPressed), type(type)
	{
		clickPosition = pos;
	}	
	MlEventTypeEnum getType() const noexcept
	{
		return type;
	}
};

struct WheelScroll
{
	MGE_EVENT_TYPE = MgeEventType::MgeWheel;
	int delta;
	WheelScroll(int scrollDelta)
		: delta(scrollDelta) {}
};

struct AppTermination
{
	MGE_EVENT_TYPE = MgeEventType::ExitApp;
};




