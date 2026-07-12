#pragma once

#include "BasicShapes.h"
#include "BasicTypes.h"

#include <variant>
#include <cmath>
#include <algorithm>
#include <vector>
#include <memory>
#include <array>
#include <crtdbg.h>

struct CellId
{
	int x;
	int y;

	bool operator==(const CellId& other) const noexcept = default;

	bool operator!=(const CellId& other) const noexcept
	{
		return !(*this == other);
	}

	CellId(int X, int Y) : x(X), y(Y) {}
};

namespace std
{
	template<>
	struct hash<CellId>
	{
		size_t operator()(const CellId& id) const noexcept
		{
			// simple but effective combination
			size_t h1 = std::hash<int>{}(id.x);
			size_t h2 = std::hash<int>{}(id.y);

			return h1 ^ (h2 << 1); // combine
		}
	};
}

namespace CollisionSystem
{
	//use Actor position and cell size to determine which cell the actor belongs to,
	// then use that cell id to store the collision component of the actor in the corresponding cell of the collision channel
	CellId getCellIdForPosition(const FPoint& position) noexcept;
	void setCellSize(mgeType::Size<size_t> newCellSize) noexcept;
	mgeType::Size<size_t> getCellSize() noexcept;
}

template<typename T>
class Trigger
{
public:
	explicit Trigger(bool enabled, SHAPE_VARIANT<T> usedShape)
		: collisionShape(std::move(usedShape)),
		isEnabled(enabled)
	{
		if (std::holds_alternative<mgeShape::Rectangle<T>>(collisionShape))
			type = SHAPE_TYPE::box;
		else if (std::holds_alternative<mgeShape::Circle<T>>(collisionShape))
			type = SHAPE_TYPE::circle;
		else 
		{
			_ASSERT(false); //unknown shape type
		}
	}

	[[nodiscard]] bool isPointInside(const mgeType::Point<T>& absolutePosition) const noexcept
	{
		if (!isEnabled)
			return false;

		if (std::holds_alternative<mgeShape::Rectangle<T>>(collisionShape))
		{
			auto& usedShape = std::get<mgeShape::Rectangle<T>>(collisionShape);
			const auto pos = usedShape.getPosition();
			const auto size = usedShape.getSize();

			// move point into local space (top-left pivot)
			const T localX = absolutePosition.x - pos.x;
			const T localY = absolutePosition.y - pos.y;

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
		else if (std::holds_alternative<mgeShape::Circle<T>>(collisionShape))
		{
			auto& usedShape = std::get<mgeShape::Circle<T>>(collisionShape);
			const auto pos = usedShape.getPosition();
			const auto radius = usedShape.getRadius();
			const T dx = absolutePosition.x - pos.x;
			const T dy = absolutePosition.y - pos.y;
			return (dx * dx + dy * dy) <= (radius * radius);
		}
		else
		{
			_ASSERT(false); //unhandled shape type
			return false;
		}
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

	void setAbsolutePosition(mgeType::Point<T> newPosition) noexcept
	{
		try
		{
			if (std::holds_alternative<mgeShape::Rectangle<T>>(collisionShape))
			{
				auto& shape = std::get<mgeShape::Rectangle<T>>(collisionShape);
				shape.setShapeAbsolutePosition(newPosition);
			}
			else if (std::holds_alternative<mgeShape::Circle<T>>(collisionShape))
			{
				auto& shape = std::get<mgeShape::Circle<T>>(collisionShape);
				shape.setShapeAbsolutePosition(newPosition);
			}
			else
			{
				_ASSERT(false); //unhandled
			}
		}
		catch (...)
		{
			_ASSERT(false);
		}
	}

	[[nodiscard]] mgeType::Point<T> getAbsolutePosition() noexcept
	{
		try
		{
			if (std::holds_alternative<mgeShape::Rectangle<T>>(collisionShape))
			{
				mgeShape::Rectangle<T> shape = std::get<mgeShape::Rectangle<T>>(collisionShape);
				return shape.getPosition();
			}
			else if (std::holds_alternative<mgeShape::Rectangle<T>>(collisionShape))
			{
				mgeShape::Circle<T> shape = std::get<mgeShape::Circle<T>>(collisionShape);
				return shape.getPosition();
			}
			else
			{
				_ASSERT(false); //unhandled
			}
		}
		catch (...)
		{
			_ASSERT(false);
		}
		return mgeType::Point<T>();
	}

	[[nodiscard]] bool getEnabled() const noexcept
	{
		return isEnabled;
	}

	[[nodiscard]] float getRotation() const noexcept
	{
		try
		{
			return std::visit([&](const auto& usedShape) -> float
				{
					using ShapeType = std::decay_t<decltype(usedShape)>;

					if constexpr (std::is_same_v<ShapeType, mgeShape::Rectangle<T>>)
						return usedShape.getRotation();
					else
					{
						_ASSERT(false); // unsupported shape
						return 0.f;
					}
				}, collisionShape);
		}
		catch (...)
		{}

		_ASSERT(false);
		return 0.f;
	}

	void setRotation(float newRotation)
	{
		std::visit([&](auto& usedShape)
			{
				using ShapeType = std::decay_t<decltype(usedShape)>;

				if constexpr (std::is_same_v<ShapeType, mgeShape::Rectangle<T>>)
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

template <typename T>
class CollisionComponent
{
public:
	CollisionComponent(T parent)
	{
		owner = parent;
	}

	void addTrigger(std::shared_ptr<Trigger<float>> trigger)
	{
		_ASSERT(trigger);
		if (!trigger)
			return;

		const auto& channels = trigger->getBlockChannels();

		for (const auto& channel : channels)
		{
			if (std::find(blockedChannels.begin(), blockedChannels.end(), channel) == blockedChannels.end())
				blockedChannels.push_back(channel);
		}

		triggers.push_back(std::move(trigger));
		setOrigin(getCenterOfComponent(), false);
		recalculateTriggerBound();
	}

	FPoint getCenterOfComponent()
	{
		std::vector<FPoint> triggerCenters;
		for (const auto& trigger : getTriggers())
		{
			if (std::holds_alternative<mgeShape::Circle<float>>(trigger->getShape()))
			{
				const auto& circle = std::get<mgeShape::Circle<float>>(trigger->getShape());
				triggerCenters.push_back(circle.getPosition());
			}
			else if (std::holds_alternative<mgeShape::Rectangle<float>>(trigger->getShape()))
			{
				const auto& rect = std::get<mgeShape::Rectangle<float>>(trigger->getShape());
				triggerCenters.push_back(rect.getCenter());
			}
			else
			{
				_ASSERT(false); //unhandled shape type
			}
		}
		FPoint averageCenter(0, 0);
		if (!triggerCenters.empty())
		{
			// Calculate the average center point
			for (const auto& center : triggerCenters)
			{
				averageCenter.x += center.x;
				averageCenter.y += center.y;
			}
			averageCenter.x /= triggerCenters.size();
			averageCenter.y /= triggerCenters.size();
		}
		else
			return origin;

		return averageCenter;
	}

	[[nodiscard]] std::vector<std::shared_ptr<Trigger<float>>>& getTriggers() noexcept
	{
		return triggers;
	}

	[[nodiscard]] bool getIsBlocking() const noexcept
	{
		return isBlocking;
	}

	[[nodiscard]] const std::vector<int>& getCollideChannels() const noexcept
	{
		return blockedChannels;
	}

	[[nodiscard]] const Trigger<float>& getBoundTrigger() const noexcept
	{
		return boundTrigger;
	}

	[[nodiscard]] float getBoundTriggerRadius() const noexcept
	{
		try
		{
			const auto& shape = boundTrigger.getShape();
			if (!std::holds_alternative<mgeShape::Circle<float>>(shape))
			{
				_ASSERT(false); //bound trigger should always be circle
				static float dummyRadius = 0.f; //to avoid warning, this line should never be reached
				return dummyRadius; //to avoid warning, this line should never be reached
			}
			return std::get<mgeShape::Circle<float>>(shape).getRadius();
		}
		catch (...)
		{
			_ASSERT(false);
		}
		return 0.f;
	}

	virtual ~CollisionComponent() = default;

	void setOwner(T newOwner) noexcept
	{
		owner = newOwner;
	}

	[[nodiscard]] T getOwner() const noexcept
	{
		return owner;
	}

	[[nodiscard]] const FPoint& getOrigin() const noexcept
	{
		return origin;
	}

	void setOrigin(const FPoint& newOrigin, bool recalculate)
	{
		origin = newOrigin;
		if (recalculate)
			recalculateCellIds();
	}

	const std::vector<CellId>& getOccupiedCells() const noexcept
	{
		return occupiedCells;
	}

	[[nodiscard]] bool getOverlapingCells() const noexcept
	{
		float r = getBoundTriggerRadius();

		auto centerCell = CollisionSystem::getCellIdForPosition(origin);

		auto overlaping = [&]( const CellId& cellId) -> bool
			{return centerCell != cellId;};

		auto leftMiddleId = CollisionSystem::getCellIdForPosition(origin + FPoint(-r, 0.f));
			if (overlaping(leftMiddleId))
				return true;
		auto rightMiddleId = CollisionSystem::getCellIdForPosition(origin + FPoint(r, 0.f));
			if (overlaping(rightMiddleId))
				return true;
		auto upMiddleId = CollisionSystem::getCellIdForPosition(origin + FPoint(0.f, -r));
			if (overlaping(upMiddleId))
				return true;
		auto downMiddleId = CollisionSystem::getCellIdForPosition(origin + FPoint(0.f, r));
			if (overlaping(downMiddleId))
				return true;

		float diag = r / std::sqrt(2.0f);

		auto leftUpId = CollisionSystem::getCellIdForPosition(origin + FPoint(-diag, -diag));
			if (overlaping(leftUpId))
				return true;
		auto rightUpId = CollisionSystem::getCellIdForPosition(origin + FPoint(diag, -diag));
			if (overlaping(rightUpId))
				return true;
		auto leftDownId = CollisionSystem::getCellIdForPosition(origin + FPoint(-diag, diag));
			if (overlaping(leftDownId))
				return true;
		auto rightDownId = CollisionSystem::getCellIdForPosition(origin + FPoint(diag, diag));
			if (overlaping(rightDownId))
				return true;
		return false;
	}

	void recalculateTriggerBound()
	{
		float radiusSum = 0.f;
		for (const auto& trgr : triggers)
		{
			if (trgr->getEnabled())
			{
				const auto& shapeOfTrigger = trgr->getShape();
				if (std::holds_alternative<mgeShape::Rectangle<float>>(shapeOfTrigger))
				{
					const auto& rect = std::get<mgeShape::Rectangle<float>>(shapeOfTrigger);
					const auto pos = rect.getPosition();     // top-left
					const auto size = rect.getSize();
					const float rot = rect.getRotation();

					constexpr float DEG_TO_RAD = 3.14159265358979323846f / 180.f;
					const float rad = rot * DEG_TO_RAD;

					const float cosA = std::cos(rad);
					const float sinA = std::sin(rad);

					// define corners in local space (relative to top-left)
					std::array<mgeType::Point<float>, 4> corners =
					{
						mgeType::Point<float>{0.f, 0.f},
						{size.width, 0.f},
						{0.f, size.height},
						{size.width, size.height}
					};

					for (const auto& c : corners)
					{
						// rotate corner
						float rotatedX = c.x * cosA - c.y * sinA;
						float rotatedY = c.x * sinA + c.y * cosA;

						// move to world space
						mgeType::Point<float> worldPoint{ pos.x + rotatedX,pos.y + rotatedY };

						float dist = tsmBasic::getDistance(worldPoint, origin);
						radiusSum = (std::max)(radiusSum, dist);
					}
				}
				else if (std::holds_alternative<mgeShape::Circle<float>>(shapeOfTrigger))
				{
					const auto& circle = std::get<mgeShape::Circle<float>>(shapeOfTrigger);

					auto dist = tsmBasic::getDistance(circle.getPosition(), origin);
					dist += circle.getRadius();
					radiusSum = (std::max)(radiusSum, dist + 1.f);
				}
				else
				{
					_ASSERT(false); //unhandled shape type
				}
			}
		}
		isBlocking = (radiusSum > 0.f);
		boundTrigger = Trigger<float>(isBlocking, mgeShape::Circle<float>(mgeType::Point<float>(origin), radiusSum));
		recalculateCellIds();
	}

protected:
	bool isBlocking = false;
	Trigger<float> boundTrigger{ false, mgeShape::Circle<float>(mgeType::Point<float>(), 0.f) };
	std::vector<std::shared_ptr<Trigger<float>>> triggers = {};

private:	
	T owner;
	FPoint origin; //absolute position

	void recalculateCellIds()
	{
		try
		{
			occupiedCells.clear();
			occupiedCells.emplace_back(CollisionSystem::getCellIdForPosition(origin)); //get cell id for current position
			float r = getBoundTriggerRadius();

			//todo: uncompleted solution, currently only consider 8 cells around the center cell
			_ASSERT(r <= CollisionSystem::getCellSize().height && r <= CollisionSystem::getCellSize().width);

			auto addCellIdIfNotExists = [&](const CellId& id)
				{
					if (std::find(occupiedCells.begin(), occupiedCells.end(), id) == occupiedCells.end())
						occupiedCells.push_back(id);
				};

			auto leftMiddleId = CollisionSystem::getCellIdForPosition(origin + FPoint(-r, 0.f));
			addCellIdIfNotExists(leftMiddleId);
			auto rightMiddleId = CollisionSystem::getCellIdForPosition(origin + FPoint(r, 0.f));
			addCellIdIfNotExists(rightMiddleId);
			auto upMiddleId = CollisionSystem::getCellIdForPosition(origin + FPoint(0.f, -r));
			addCellIdIfNotExists(upMiddleId);
			auto downMiddleId = CollisionSystem::getCellIdForPosition(origin + FPoint(0.f, r));
			addCellIdIfNotExists(downMiddleId);

			float diag = r / std::sqrt(2.0f);

			auto leftUpId = CollisionSystem::getCellIdForPosition(origin + FPoint(-diag, -diag));
			addCellIdIfNotExists(leftUpId);
			auto rightUpId = CollisionSystem::getCellIdForPosition(origin + FPoint(diag, -diag));
			addCellIdIfNotExists(rightUpId);
			auto leftDownId = CollisionSystem::getCellIdForPosition(origin + FPoint(-diag, diag));
			addCellIdIfNotExists(leftDownId);
			auto rightDownId = CollisionSystem::getCellIdForPosition(origin + FPoint(diag, diag));
			addCellIdIfNotExists(rightDownId);
		}
		catch (...)
		{
			_ASSERT(false);
		}
	}

	std::vector<CellId> occupiedCells;
	std::vector<int> blockedChannels;
};

