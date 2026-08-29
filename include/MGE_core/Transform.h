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

	[[nodiscard]] float getRotation() const noexcept
	{
		return m_rotation;
	}

	void setRotation(float rotation)
	{
		m_rotation = rotation;
	}

protected:
	FPoint m_position;
	float m_rotation = 0.f;
	FSize m_size;
};

