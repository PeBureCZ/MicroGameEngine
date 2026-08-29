#pragma once
#include <vadefs.h> //uintptr_t

using MgeObjectId = uintptr_t;

class MgeObject
{
public:
	MgeObject() = default;

	virtual MgeObjectId getId() const noexcept;

private:

};

