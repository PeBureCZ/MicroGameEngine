#pragma once
#include "BasicTypes.h"

class BasicTransform
{
public:
	BasicTransform() = default;

	[[nodiscard]] const FPoint& getPosition() const noexcept
	{
		return m_position;
	}

	void setPosition(const FPoint& position)
	{
		m_position = position;
	}

	[[nodiscard]] const FSize& getSize() const noexcept
	{
		return m_size;
	}

	void setSize(const FSize& size) noexcept
	{
		m_size = size;
	}

protected:
	FPoint m_position;
	float rotation = 0.f;
	FSize m_size;
};

