#pragma once

#include "..\include\MGE_GUI\MgeFrame.h"

constexpr IPoint startFrameTestPosition(23, 785);
constexpr mgeType::Size<int> startFrameTestSize(201, 335);

class MgeFrame_test : public MgeFrame
{
public:
	MgeFrame_test() : MgeFrame(startFrameTestPosition, startFrameTestSize)
	{

	}

	void mock_layout() noexcept
	{
		MgeFrame::layout();
	}

	[[nodiscard]] IPoint mock_getLastLayoutAbsolutePosition() const noexcept
	{
		return lastLayoutAbsolutePosition;
	}
};