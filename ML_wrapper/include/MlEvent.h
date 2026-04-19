#pragma once

#include <variant>

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
	int delta;
	WheelScroll(int scrollDelta)
		: delta(scrollDelta) {}
};

struct AppTermination
{

};

using MlEventType = std::variant<MouseClick, WheelScroll, AppTermination>;

class MlEvent
{
private:
	MlEventType data;
public:
	template<typename T>
	MlEvent(T&& value)
		: data(std::forward<T>(value)) {}
	
};

