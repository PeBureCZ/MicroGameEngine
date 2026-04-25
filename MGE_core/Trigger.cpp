#include "Trigger.h"

void TriggerManager::addTrigger(std::shared_ptr<Trigger<float>> trigger, bool recalculate)
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
	if (recalculate)
		recalculateTriggerBound();
}

void TriggerManager::addTriggers(std::vector<std::shared_ptr<Trigger<float>>> triggers)
{
	for (auto& trigger : triggers)
		addTrigger(std::move(trigger), false);
	recalculateTriggerBound();
}

[[nodiscard]] std::vector<std::shared_ptr<Trigger<float>>>& TriggerManager::getTriggers() noexcept
{
	return triggers;
}

[[nodiscard]] bool TriggerManager::getIsBlocking() const noexcept
{
	return isBlocking;
}

const std::vector<int>& TriggerManager::getCollideChannels() const noexcept
{
	return blockedChannels;
}

void TriggerManager::recalculateTriggerBound()
{
	float radiusSum = 0.f;
	for (const auto& trgr : triggers)
	{
		if (trgr->getEnabled())
		{
			const auto& shapeOfTrigger = trgr->getShape();
			if (std::holds_alternative<tsmShape::Rectangle<float>>(shapeOfTrigger))
			{
				const auto& rect = std::get<tsmShape::Rectangle<float>>(shapeOfTrigger);
				const auto pos = rect.getPosition();     // top-left
				const auto size = rect.getSize();
				const float rot = rect.getRotation();

				constexpr float DEG_TO_RAD = 3.14159265358979323846f / 180.f;
				const float rad = rot * DEG_TO_RAD;

				const float cosA = std::cos(rad);
				const float sinA = std::sin(rad);

				// define corners in local space (relative to top-left)
				std::array<tsmType::Point<float>, 4> corners =
				{
					tsmType::Point<float>{0.f, 0.f},
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
					tsmType::Point<float> worldPoint{ pos.x + rotatedX,pos.y + rotatedY };

					float dist = tsmBasic::getDistance(worldPoint, origin);
					radiusSum = std::max(radiusSum, dist);
				}
			}
			else if (std::holds_alternative<tsmShape::Circle<float>>(shapeOfTrigger))
			{
				const auto& circle = std::get<tsmShape::Circle<float>>(shapeOfTrigger);

				auto dist = tsmBasic::getDistance(circle.getPosition(), origin);
				dist += circle.getRadius();
				radiusSum = std::max(radiusSum, dist);
				radiusSum += (dist + 1.f);
			}
			else
			{
				_ASSERT(false); //unhandled shape type
			}
		}
	}
	isBlocking = (radiusSum > 0.f);
	boundTrigger = Trigger<float>(isBlocking, tsmShape::Circle<float>(tsmType::Point<float>(), radiusSum));
}