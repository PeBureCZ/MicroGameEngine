#pragma once

#include "..\MGE_GUI\include\Frame.h"
//#include "Widget_test.h"

constexpr IPoint startFrameTestPosition(23, 785);
constexpr mgeType::Size<int> startFrameTestSize(201, 335);

class Frame_test : public Frame
{
public:
	Frame_test() : Frame(startFrameTestPosition, startFrameTestSize)
	{

	}

	void mock_layout() noexcept
	{
		Frame::layout();
	}

	[[nodiscard]] IPoint mock_getLastLayoutAbsolutePosition() const noexcept
	{
		return lastLayoutAbsolutePosition;
	}
};