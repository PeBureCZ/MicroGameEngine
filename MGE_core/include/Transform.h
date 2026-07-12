#pragma once
#include "BasicTypes.h"

class BasicTransform
{
public:
	BasicTransform() = default;
protected:
	FPoint position;
	float rotation = 0.f;
	float size = 1.f;
};

