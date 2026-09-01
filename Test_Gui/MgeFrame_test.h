#pragma once

#include "..\include\MGE_GUI\MgeFrame.h"

constexpr FPoint startFrameTestPosition(23.f, 785.f);
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

	[[nodiscard]] FPoint mock_getLastLayoutAbsolutePosition() const noexcept
	{
		return lastLayoutAbsolutePosition;
	}
};