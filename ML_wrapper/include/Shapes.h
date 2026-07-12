#pragma once
#include <vector>

#include "basicTypes.h"

namespace mgeShape
{
	template<typename T>
	class Shape
	{
	protected:
		float rotation = 0.0f; // in degrees
		mgeType::Point<T> position = { 0.0f, 0.0f };
	public:
		virtual ~Shape() = default;
	};

	template<typename T>
	class Rectangle : public Shape
	{
	private:
		T width = 1.0;
		T height = 1.0;
	public:
		Rectangle(T width, T height, float newRotation = 0.0f);
	};
}
