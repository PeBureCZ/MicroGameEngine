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

namespace mgeShape
{
	template<typename T>
	class Shape
	{
	public:
		Shape(mgeType::Point<T> newPosition = mgeType::Point<T>(), float newRotation = 0.0f)
			: absolutePosition(newPosition), rotation(newRotation)
		{}
		mgeType::Point<T> getPosition() const { return absolutePosition; }
		void setShapeAbsolutePosition(mgeType::Point<T> newPosition) noexcept
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
		mgeType::Point<T> absolutePosition; // default position at origin
		float rotation = 0.0f; // in degrees
		SHAPE_TYPE type = SHAPE_TYPE::UNDEFINED;
	};

	template<typename T>
	class Rectangle : public Shape<T>
	{
	private:
	protected:
		mgeType::Size<T> size;
	public:
		Rectangle()
			: Shape<T>(mgeType::Point<T>(), 0.0)
		{
			size = mgeType::Size<T>();
			Shape<T>::type = SHAPE_TYPE::box;
		};
		Rectangle(mgeType::Point<T> newPosition, mgeType::Size<T> newSize, float newRotation = 0.0f)
			: Shape<T>(newPosition, newRotation)
		{
			size = newSize;
			Shape<T>::type = SHAPE_TYPE::box;
		};

		mgeType::Size<T> getSize() const { return size; }

		//return the center of the rectangle, taking into account its rotation (absolute position)
		mgeType::Point<T> getCenter() const
		{
			auto rotation = Shape<T>::getRotation();
			if (rotation == 0.0f)
				return mgeType::Point<T>(Shape<T>::getPosition().x + size.width / 2, Shape<T>::getPosition().y + size.height / 2);
			else
			{
				constexpr float DEG_TO_RAD = 3.14159265358979323846f / 180.f;
				const float rad = rotation * DEG_TO_RAD;
				const float cosA = std::cos(rad);
				const float sinA = std::sin(rad);
				// Calculate the center of the rectangle before rotation
				mgeType::Point<T> centerBeforeRotation(Shape<T>::getPosition().x + size.width / 2, Shape<T>::getPosition().y + size.height / 2);
				// Rotate the center point around the rectangle's position
				T rotatedX = (centerBeforeRotation.x - Shape<T>::getPosition().x) * cosA - (centerBeforeRotation.y - Shape<T>::getPosition().y) * sinA + Shape<T>::getPosition().x;
				T rotatedY = (centerBeforeRotation.x - Shape<T>::getPosition().x) * sinA + (centerBeforeRotation.y - Shape<T>::getPosition().y) * cosA + Shape<T>::getPosition().y;
				return mgeType::Point<T>(rotatedX, rotatedY);
			}
		}
	};

	template<typename T>
	class Circle : public Shape<T>
	{
	private:
	protected:
		T radius;
	public:
		Circle(mgeType::Point<T> newPosition, T newRadius)
			: Shape<T>(newPosition, 0.f), radius(newRadius)
		{
			Shape<T>::type = SHAPE_TYPE::circle;
		};

		T getRadius() const noexcept
		{ return radius; }
	};
}

template<typename T>
using SHAPE_VARIANT = std::variant<mgeShape::Rectangle<T>,mgeShape::Circle<T>>;

template<typename T>
class DrawableObject
{
public:

	DrawableObject()
		: m_shape(mgeShape::Rectangle<T>())
	{}

	DrawableObject(SHAPE_VARIANT<T> shape, mgeType::Color_RGBA usedColor = mgeType::Color_RGBA())
		: m_shape(shape), color(usedColor) // default white color
	{

	}

	const SHAPE_VARIANT<T>& getShape() const
	{
		return m_shape;
	}

	mgeType::Color_RGBA getColor() const
	{
		return color;
	}

	void setColor(mgeType::Color_RGBA& newColor)
	{
		color = newColor;
	}
	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		color = mgeType::Color_RGBA(r,g,b,a);
	}

	mgeType::Point<T> getPosition()
	{
		if (std::holds_alternative<mgeShape::Rectangle<T>>(m_shape))
		{
			auto& obj = std::get<mgeShape::Rectangle<T>>(m_shape);
			return obj.getPosition();
		}

		else if (std::holds_alternative<mgeShape::Circle<T>>(m_shape))
		{
			auto& obj = std::get<mgeShape::Circle<T>>(m_shape);
			return obj.getPosition();
		}
		return mgeType::Point<T>();
	}

	void setAbsoluteDrawablePosition(mgeType::Point<T> newPosition)
	{
		if (std::holds_alternative<mgeShape::Rectangle<T>>(m_shape))
		{
			auto& obj = std::get<mgeShape::Rectangle<T>>(m_shape);
			obj.setShapeAbsolutePosition(std::move(newPosition));
		}
		else if (std::holds_alternative<mgeShape::Circle<T>>(m_shape))
		{
			auto& obj = std::get<mgeShape::Circle<T>>(m_shape);
			obj.setShapeAbsolutePosition(std::move(newPosition));
		}
		else
		{
			_ASSERT(false); //unhandled
		}
	}

	T getRotation() const
	{
		if (std::holds_alternative<mgeShape::Rectangle<T>>(m_shape))
		{
			auto& obj = std::get<mgeShape::Rectangle<T>>(m_shape);
			return obj.getRotation();
		}
		else if (std::holds_alternative<mgeShape::Circle<T>>(m_shape))
		{
			auto& obj = std::get<mgeShape::Circle<T>>(m_shape);
			return obj.getRotation();
		}
		return 0;
	}

	void setRotation(float newRotation)
	{
		if (std::holds_alternative<mgeShape::Rectangle<T>>(m_shape))
		{
			auto& obj = std::get<mgeShape::Rectangle<T>>(m_shape);
			obj.setRotation(newRotation);
		}
		else if (std::holds_alternative<mgeShape::Circle<T>>(m_shape))
		{
			auto& obj = std::get<mgeShape::Circle<T>>(m_shape);
			obj.setRotation(newRotation);
		}
		else
		{
			_ASSERT(false); //unhandled
		}
	}

	virtual ~DrawableObject() = default;

protected:
	mgeType::Color_RGBA color;
	SHAPE_VARIANT<T> m_shape;

};
