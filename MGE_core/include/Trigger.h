#pragma once

#include "BasicShapes.h"
#include "BasicTypes.h"

#include <variant>
#include <cmath>
#include <vector>

template<typename T>
class Trigger
{
public:
	explicit Trigger(bool enabled, SHAPE_VARIANT<T> usedShape)
		: collisionShape(std::move(usedShape)),
		isEnabled(enabled)
	{
		if (std::holds_alternative<tsmShape::Rectangle<T>>(collisionShape))
			type = SHAPE_TYPE::box;
		else if (std::holds_alternative<tsmShape::Circle<T>>(collisionShape))
			type = SHAPE_TYPE::circle;
		else 
		{
			_ASSERT(false); //unknown shape type
		}
	}

	[[nodiscard]] bool isPointInside(const tsmType::Point<T>& point) const noexcept
	{
		if (!isEnabled)
			return false;

		return std::visit([&](const auto& usedShape) -> bool
			{
				using ShapeType = std::decay_t<decltype(usedShape)>;

				if constexpr (std::is_same_v<ShapeType, tsmShape::Rectangle<T>>)
				{
					const auto pos = usedShape.getPosition();
					const auto size = usedShape.getSize();

					// move point into local space (top-left pivot)
					const T localX = point.x - pos.x;
					const T localY = point.y - pos.y;

					// rotate point back
					constexpr float DEG_TO_RAD = 3.14159265358979323846f / 180.f;
					const float rad = - getRotation() * DEG_TO_RAD;
					const float cosA = std::cos(rad);
					const float sinA = std::sin(rad);

					const T rotatedX = static_cast<T>(localX * cosA - localY * sinA);
					const T rotatedY = static_cast<T>(localX * sinA + localY * cosA);

					// AABB test in local space
					return rotatedX >= static_cast<T>(0) &&
						rotatedX <= size.width &&
						rotatedY >= static_cast<T>(0) &&
						rotatedY <= size.height;
				}
				else if (std::is_same_v<ShapeType, tsmShape::Circle<T>>)
				{
					const auto pos = usedShape.getPosition();
					const auto radius = usedShape.getRadius();
					const T dx = point.x - pos.x;
					const T dy = point.y - pos.y;
					return (dx * dx + dy * dy) <= (radius * radius);
				}
				else
				{
					_ASSERT(false); // unsupported shape
					return false;
				}
			}, collisionShape);
	}

	void setNewShape(const SHAPE_VARIANT<T>& newShape) noexcept
	{
		collisionShape = newShape;
	}

	[[nodiscard]] const SHAPE_VARIANT<T>& getShape() const noexcept
	{
		return collisionShape;
	}

	[[nodiscard]] SHAPE_TYPE getType() const noexcept
	{
		return std::visit([&](const auto& usedShape) -> SHAPE_TYPE
			{
				return usedShape.getType();
			}, collisionShape);
	}

	void setEnabled(bool enabled) noexcept
	{
		isEnabled = enabled;
	}

	void setAbsolutePosition(tsmType::Point<T> newPosition) noexcept
	{
		if (std::holds_alternative<tsmShape::Rectangle<T>>(collisionShape))
		{
			auto& shape = std::get<tsmShape::Rectangle<T>>(collisionShape);
			shape.setShapeAbsolutePosition(newPosition);
		}
		else if (std::holds_alternative<tsmShape::Circle<T>>(collisionShape))
		{
			auto& shape = std::get<tsmShape::Circle<T>>(collisionShape);
			shape.setShapeAbsolutePosition(newPosition);
		}
		else
		{
			_ASSERT(false); //unhandled
		}
	}

	[[nodiscard]] tsmType::Point<T> getAbsolutePosition() noexcept
	{
		if (std::holds_alternative<tsmShape::Rectangle<T>>(collisionShape))
		{
			tsmShape::Rectangle<T> shape = std::get<tsmShape::Rectangle<T>>(collisionShape);
			return shape.getPosition();
		}
		else if (std::holds_alternative<tsmShape::Rectangle<T>>(collisionShape))
		{
			tsmShape::Circle<T> shape = std::get<tsmShape::Circle<T>>(collisionShape);
			return shape.getPosition();
		}
		else
		{
			_ASSERT(false); //unhandled
		}
		return tsmType::Point<T>();
	}

	[[nodiscard]] bool getEnabled() const noexcept
	{
		return isEnabled;
	}

	[[nodiscard]] float getRotation() const noexcept
	{
		return std::visit([&](const auto& usedShape) -> float
			{
				using ShapeType = std::decay_t<decltype(usedShape)>;

				if constexpr (std::is_same_v<ShapeType, tsmShape::Rectangle<T>>)
					return usedShape.getRotation();
				else
				{
					_ASSERT(false); // unsupported shape
					return 0.f;
				}
			}, collisionShape);
	}
	void setRotation(float newRotation)
	{
		std::visit([&](auto& usedShape)
			{
				using ShapeType = std::decay_t<decltype(usedShape)>;

				if constexpr (std::is_same_v<ShapeType, tsmShape::Rectangle<T>>)
					usedShape.setRotation(newRotation);
				else
				{
					_ASSERT(false); // unsupported shape
				}
			}, collisionShape);
	}

	~Trigger() = default;

	[[nodiscard]] bool getBlocking() const noexcept
	{
		return isBlocking;
	}

	void setIsBlocking(bool newBlocking) noexcept
	{
		isBlocking = newBlocking;
	}

	void setBlockChannels(std::vector<int> newBlockChannels)
	{
		blockedChannels = std::move(newBlockChannels);
	}

	[[nodiscard]] const std::vector<int>& getBlockChannels() const noexcept
	{
		return blockedChannels;
	}

private:
	SHAPE_VARIANT<T> collisionShape;
	//float rotation = 0.f;
	bool isEnabled = false;
	bool isBlocking = true;
	SHAPE_TYPE type = SHAPE_TYPE::box;
	std::vector<int> blockedChannels;
};

