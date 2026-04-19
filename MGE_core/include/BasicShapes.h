#pragma once
#include <vector>
#include <variant>
#include "BasicTypes.h"

enum class SHAPE_TYPE
{
	box,
	circle,
	UNDEFINED
};

namespace tsmShape
{
	template<typename T>
	class Shape
	{
	public:
		Shape(tsmType::Point<T> newPosition = tsmType::Point<T>(), float newRotation = 0.0f)
			: absolutePosition(newPosition), rotation(newRotation)
		{}
		tsmType::Point<T> getPosition() const { return absolutePosition; }
		void setShapeAbsolutePosition(tsmType::Point<T> newPosition) noexcept
		{ 
			absolutePosition = newPosition;
		}

		float getRotation() const noexcept
		{
			return rotation;
		}

		void setRotation(float newRotation)
		{
			rotation = newRotation;
		}

		SHAPE_TYPE getType() const noexcept
		{
			return type;
		}

		virtual ~Shape() = default;
	protected:
		tsmType::Point<T> absolutePosition; // default position at origin
		float rotation = 0.0f; // in degrees
		SHAPE_TYPE type = SHAPE_TYPE::UNDEFINED;
	};

	template<typename T>
	class Rectangle : public Shape<T>
	{
	private:
	protected:
		tsmType::Size<T> size;
	public:
		Rectangle()
			: Shape<T>(tsmType::Point<T>(), 0.0)
		{
			size = tsmType::Size<T>();
			Shape<T>::type = SHAPE_TYPE::box;
		};
		Rectangle(tsmType::Point<T> newPosition, tsmType::Size<T> newSize, float newRotation = 0.0f)
			: Shape<T>(newPosition, newRotation)
		{
			size = newSize;
			Shape<T>::type = SHAPE_TYPE::box;
		};

		tsmType::Size<T> getSize() const { return size; }
	};

	template<typename T>
	class Circle : public Shape<T>
	{
	private:
	protected:
		T radius;
	public:
		Circle(tsmType::Point<T> newPosition, T newRadius)
			: Shape<T>(newPosition, 0.f)
		{
			radius = newRadius;
		};

		T getRadius() const { return radius; }
	};
}

template<typename T>
using SHAPE_VARIANT = std::variant<tsmShape::Rectangle<T>,tsmShape::Circle<T>>;

template<typename T>
class DrawableObject
{
public:
	DrawableObject(SHAPE_VARIANT<T> shape, tsmType::Color_RGBA usedColor = tsmType::Color_RGBA())
		: shape(shape), color(usedColor) // default white color
	{

	}

	SHAPE_VARIANT<T>& getShape()
	{
		return shape;
	}

	tsmType::Color_RGBA getColor() const
	{
		return color;
	}

	void setColor(tsmType::Color_RGBA& newColor)
	{
		color = newColor;
	}
	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		color = tsmType::Color_RGBA(r,g,b,a);
	}

	tsmType::Point<T> getPosition()
	{
		if (std::holds_alternative<tsmShape::Rectangle<T>>(shape))
		{
			auto& obj = std::get<tsmShape::Rectangle<T>>(shape);
			return obj.getPosition();
		}

		else if (std::holds_alternative<tsmShape::Circle<T>>(shape))
		{
			auto& obj = std::get<tsmShape::Circle<T>>(shape);
			return obj.getPosition();
		}
		return tsmType::Point<T>();
	}

	void setAbsoluteDrawablePosition(tsmType::Point<T> newPosition)
	{
		if (std::holds_alternative<tsmShape::Rectangle<T>>(shape))
		{
			auto& obj = std::get<tsmShape::Rectangle<T>>(shape);
			obj.setShapeAbsolutePosition(std::move(newPosition));
		}
		else if (std::holds_alternative<tsmShape::Circle<T>>(shape))
		{
			auto& obj = std::get<tsmShape::Circle<T>>(shape);
			obj.setShapeAbsolutePosition(std::move(newPosition));
		}
		else
		{
			_ASSERT(false); //unhandled
		}
	}

	T getRotation() const
	{
		if (std::holds_alternative<tsmShape::Rectangle<T>>(shape))
		{
			auto& obj = std::get<tsmShape::Rectangle<T>>(shape);
			return obj.getRotation();
		}
		else if (std::holds_alternative<tsmShape::Circle<T>>(shape))
		{
			auto& obj = std::get<tsmShape::Circle<T>>(shape);
			return obj.getRotation();
		}
		return 0;
	}

	void setRotation(float newRotation)
	{
		if (std::holds_alternative<tsmShape::Rectangle<T>>(shape))
		{
			auto& obj = std::get<tsmShape::Rectangle<T>>(shape);
			obj.setRotation(newRotation);
		}
		else if (std::holds_alternative<tsmShape::Circle<T>>(shape))
		{
			auto& obj = std::get<tsmShape::Circle<T>>(shape);
			obj.setRotation(newRotation);
		}
		else
		{
			_ASSERT(false); //unhandled
		}
	}

	virtual ~DrawableObject() = default;

protected:
	tsmType::Color_RGBA color;
	SHAPE_VARIANT<T> shape;

};
