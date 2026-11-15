#include "MgeDefaultComponent.h"

#include "MgeActor.h"

static MgeDefaultComponent ERROR_STATE_COMPONENT;

void MgeDefaultComponent::addChild(const std::shared_ptr<MgeActor>& child, std::optional<size_t> toIndex) noexcept
{
	try
	{
		_ASSERT(child);
		if (!child)
			return;

		if (toIndex.has_value())
		{
			auto index = toIndex.value();
			_ASSERT(index <= m_children.size());
			if (index >= m_children.size())
				m_children.push_back(child);
			else
				m_children.insert(m_children.begin() + index, child);
		}
		else
		{
			m_children.push_back(child);
		}
	}
	catch (...)
	{
		_ASSERT(false); //unexpected exception
	}
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

[[nodiscard]] std::shared_ptr<MgeActor> MgeDefaultComponent::editParent() noexcept
{
	return parent.lock();
}

[[nodiscard]] const std::shared_ptr<MgeActor> MgeDefaultComponent::getParent() const noexcept
{
	return parent.lock();
}

const std::shared_ptr<MgeActor> MgeDefaultComponent::getMasterParent() const noexcept
{
	if (parent.expired())
		return parent.lock();
	else
	{
		auto p = parent.lock();
		if (p->getParent())
			return parent.lock()->getMasterParent();
		return parent.lock();
	}
}

[[nodiscard]] bool MgeDefaultComponent::removeChild(std::shared_ptr<MgeActor> child)
{
	for (auto it = m_children.begin(); it != m_children.end(); ++it)
	{
		if (auto& checkedChild = *it)
		{
			if (checkedChild && checkedChild == child)
			{
				checkedChild->editMgeDefaultComponent().setParent();
				m_children.erase(it); //removal only detaches the child; destroying is done by mge::destroyActor
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
				m_children.erase(it); //removal only detaches the child; destroying is done by mge::destroyActor
				return true;
			}
		}
	}
	_ASSERT(false); //try to remove a non-existent child
	return false;
}

[[nodiscard]] FPoint MgeDefaultComponent::getAbsolutePosition() const noexcept
{
	if (const auto parentActor = parent.lock())
		return getPosition() + parentActor->getAbsolutePosition();
	return getPosition();
}

[[nodiscard]] const FPoint& MgeDefaultComponent::getPosition() const noexcept
{
	return MgeTransform::getPosition();
}

void MgeDefaultComponent::setRelativePosition(const FPoint& position) noexcept
{
	setPosition(position);
}

void MgeDefaultComponent::setAbsolutePosition(const FPoint& position) noexcept
{
	if (const auto parentActor = parent.lock())
		MgeTransform::setPosition(position - parentActor->getAbsolutePosition());
	else
		MgeTransform::setPosition(position);
}

[[nodiscard]] float MgeDefaultComponent::getAbsoluteRotation() const noexcept
{
	if (const auto parentActor = parent.lock())
		return getRotation() + parentActor->getAbsoluteRotation();
	return getRotation();
}

[[nodiscard]] float MgeDefaultComponent::getRelativeRotation() const noexcept
{
	return getRotation();
}

void MgeDefaultComponent::setAbsoluteRotation(float rotation) noexcept
{
	if (const auto parentActor = parent.lock())
		setRotation(rotation - parentActor->getAbsoluteRotation());
	else
		setRotation(rotation);
}

void MgeDefaultComponent::setRelativeRotation(float rotation) noexcept
{
	setRotation(rotation);
}

MgeDefaultComponent& MgeActor::editMgeDefaultComponent() noexcept
{
	if (defaultActorData)
		return *defaultActorData.get();
	_ASSERT(false);
	createMgeDefaultComponent();
	return ERROR_STATE_COMPONENT;
}

const MgeDefaultComponent& MgeActor::getMgeDefaultComponent() const noexcept
{
	if (defaultActorData)
		return *defaultActorData;
	_ASSERT(false);
	return ERROR_STATE_COMPONENT;
}