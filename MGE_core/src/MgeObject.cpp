#include "MgeObject.h"

MgeObjectId MgeObject::getId() const noexcept
{
	return reinterpret_cast<MgeObjectId>(this);
}
