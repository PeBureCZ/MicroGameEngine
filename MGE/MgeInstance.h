#pragma once
#include <vadefs.h> //uintptr_t

using MgeInstanceId = uintptr_t;

class MgeInstance
{
public:
	MgeInstance() = default;

	virtual MgeInstanceId getId()
	{
		return reinterpret_cast<MgeInstanceId>(this);
	}

private:

};
