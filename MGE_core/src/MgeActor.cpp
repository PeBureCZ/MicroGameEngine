#include "MgeActor.h"

void MgeDefaultComponent::addChild(const std::shared_ptr<MgeActor>& child) noexcept
{
	m_children.push_back(child);
}

void MgeDefaultComponent::setParent(const std::shared_ptr<MgeActor>& newParent) noexcept
{
	parent = newParent;
}


[[nodiscard]] const std::vector<std::shared_ptr<MgeActor>>& MgeDefaultComponent::getChildren() const noexcept
{
	return m_children;
}

[[nodiscard]] std::vector<std::shared_ptr<MgeActor>>& MgeDefaultComponent::editChildren() noexcept
{
	return m_children;
}

[[nodiscard]] std::optional<std::shared_ptr<MgeActor>> MgeDefaultComponent::editParent() noexcept
{
	if (!parent.expired())
		return parent.lock();
	return std::nullopt;
}

[[nodiscard]] std::optional<const std::shared_ptr<MgeActor>> MgeDefaultComponent::getParent() const noexcept
{
	if (parent.expired())
		return std::nullopt;
	return parent.lock();
}

[[nodiscard]] bool MgeDefaultComponent::removeChild(std::shared_ptr<MgeActor>& child)
{
	for (auto it = m_children.begin(); it != m_children.end(); ++it)
	{
		if (auto& checkedChild = *it)
		{
			if (checkedChild && checkedChild == child)
			{
				checkedChild->editMgeDefaultComponent().setParent();
				m_children.erase(it);
				return true;
			}
		}
	}
	_ASSERT(false); //try to remove a non-existent child
	return false;
}

[[nodiscard]] bool MgeDefaultComponent::removeChild(MgeObjectId childId)
{
	for (auto it = m_children.begin(); it != m_children.end(); ++it)
	{
		if (auto& checkedChild = *it)
		{
			if (checkedChild && checkedChild->getId() == childId)
			{
				checkedChild->editMgeDefaultComponent().setParent();
				m_children.erase(it);
				return true;
			}
		}
	}
	_ASSERT(false); //try to remove a non-existent child
	return false;
}

[[nodiscard]] FPoint MgeDefaultComponent::getAbsolutePosition() const noexcept
{
	if (!parent.expired())
		return { getPosition() + parent.lock()->getAbsolutePosition() };
	else
		return getPosition();
}

[[nodiscard]] const FPoint& MgeDefaultComponent::getRelativePosition() const noexcept
{
	return getPosition();
}

void MgeDefaultComponent::setRelativePosition(const FPoint& position) noexcept
{
	setPosition(position);
}

void MgeDefaultComponent::setAbsolutePosition(const FPoint& position) noexcept
{
	const auto& actualPos = getAbsolutePosition();
	auto dif = position - actualPos;
	MgeTransform::setPosition(position);
}

//##############		MgeBasicActor		##############

const std::vector<std::shared_ptr<MgeBasicComponent>>& MgeBasicActor::getComponents() const noexcept
{
	return m_components;
}

std::vector<std::shared_ptr<MgeBasicComponent>>& MgeBasicActor::editComponents() noexcept
{
	return m_components;
}

std::optional<std::shared_ptr<MgeBasicComponent>> MgeBasicActor::editComponent(ComponentType type) noexcept
{
	for (auto& component : editComponents())
	{
		if (component && component->getType() == type)
			return component;
	}
	return std::nullopt;
}

//##############		MgeActor			##############

void MgeActor::setRelativePosition(const FPoint& newPosition) noexcept
{
	defaultActorData.setRelativePosition(newPosition);
}

void MgeActor::setAbsolutePosition(const FPoint& newPosition) noexcept
{
	defaultActorData.setAbsolutePosition(newPosition);
}

[[nodiscard]] const FPoint& MgeActor::getRelativePosition() const noexcept
{
	return defaultActorData.getRelativePosition();
}

[[nodiscard]] FPoint MgeActor::getAbsolutePosition() const noexcept
{
	return defaultActorData.getAbsolutePosition();
}

MgeDefaultComponent& MgeActor::editMgeDefaultComponent() noexcept
{
	return defaultActorData;
}

const MgeDefaultComponent& MgeActor::getMgeDefaultComponent() const noexcept
{
	return defaultActorData;
}



