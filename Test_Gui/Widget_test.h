#pragma once
#include "..\MGE_GUI\include\Widget.h"

constexpr IPoint startTestPosition(23, 785);
constexpr mgeType::Size<int> startTestSize(201, 335);

class Widget_test : public Widget
{
public:
	Widget_test() : Widget(startTestPosition, startTestSize)
	{

	}

	std::weak_ptr<Widget> mock_getSelfPtr() const noexcept
	{
		return getSelfPtr();
	}
};