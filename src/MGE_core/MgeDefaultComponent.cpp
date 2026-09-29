#include "MgeDefaultComponent.h"

#include "MgeActor.h"

static MgeDefaultComponent ERROR_STATE_COMPONENT;

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
#ifdef _DEBUG
				[[maybe_unused]] auto count = child.use_count();
				_ASSERT(count == 1); //input shared_ptr should be the last live element
#endif //_DEBUG
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
#ifdef _DEBUG
				[[maybe_unused]] auto count = checkedChild.use_count();
				_ASSERT(count == 1); //no elements should remain alive AFTER erase
#endif //_DEBUG
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
	const auto& actualPos = getAbsolutePosition();
	auto dif = position - actualPos;
	MgeTransform::setPosition(position);
}

[[nodiscard]] float MgeDefaultComponent::getAbsoluteRotation() const noexcept
{
	if (!parent.expired())
		return { getRotation() + parent.lock()->getAbsoluteRotation() };
	else
		return getRotation();
}

[[nodiscard]] float MgeDefaultComponent::getRelativeRotation() const noexcept
{
	return getRotation();
}

void MgeDefaultComponent::setAbsoluteRotation(float rotation) noexcept
{
	float actualAbsoluteRotation = (parent.expired())
		? getRotation()
		: getRotation() + parent.lock()->getAbsoluteRotation();

	setRotation(rotation - actualAbsoluteRotation);
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