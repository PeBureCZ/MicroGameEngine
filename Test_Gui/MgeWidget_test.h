#pragma once
#include "..\include\MGE_GUI\MgeWidget.h"

constexpr FPoint startTestPosition(23.f, 785.f);
constexpr mgeType::Size<int> startTestSize(201, 335);

class MgeWidget_test : public MgeWidget
{
public:
	MgeWidget_test() : MgeWidget(startTestPosition, startTestSize)
	{

	}

	std::weak_ptr<MgeWidget> mock_getSelfPtr() const noexcept
	{
		return getSelfPtr();
	}
};