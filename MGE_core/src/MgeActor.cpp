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

void MgeBasicActor::addComponent(std::shared_ptr<MgeBasicComponent> newComponent)
{
	m_components.push_back(newComponent);
}

//##############		MgeActor			##############

void MgeActor::setRelativePosition(const FPoint& newPosition) noexcept
{
	editMgeDefaultComponent().setRelativePosition(newPosition);
}

void MgeActor::setAbsolutePosition(const FPoint& newPosition) noexcept
{
	editMgeDefaultComponent().setAbsolutePosition(newPosition);
}

void MgeActor::setParent(const std::shared_ptr<MgeActor>& newParent) noexcept
{
	editMgeDefaultComponent().setParent(newParent);
}

[[nodiscard]] std::optional<const std::shared_ptr<MgeActor>> MgeActor::getParent() const noexcept
{
	return getMgeDefaultComponent().getParent();
}

[[nodiscard]] const std::vector<std::shared_ptr<MgeActor>>& MgeActor::getChildren() const noexcept
{
	return getMgeDefaultComponent().getChildren();
}

std::vector<std::shared_ptr<MgeActor>>& MgeActor::editChildren() noexcept
{
	return editMgeDefaultComponent().editChildren();
}

void MgeActor::addChild(const std::shared_ptr<MgeActor>& child) noexcept
{
	editMgeDefaultComponent().addChild(child);
}

[[nodiscard]] bool MgeActor::removeChild(std::shared_ptr<MgeActor>& child)
{
	return editMgeDefaultComponent().removeChild(child);
}

[[nodiscard]] bool MgeActor::removeChild(MgeObjectId childId)
{
	return editMgeDefaultComponent().removeChild(childId);
}

[[nodiscard]] const FPoint& MgeActor::getRelativePosition() const noexcept
{
	return getMgeDefaultComponent().getRelativePosition();
}

[[nodiscard]] FPoint MgeActor::getAbsolutePosition() const noexcept
{
	return getMgeDefaultComponent().getAbsolutePosition();
}

void MgeActor::setRelativeRotation(float rotation)
{
	return editMgeDefaultComponent().setRotation(rotation);
}

[[nodiscard]] float MgeActor::getRelativeRotation() const noexcept
{
	return getMgeDefaultComponent().getRotation();
}

[[nodiscard]] float MgeActor::getAbsoluteRotation() const noexcept
{
	return getMgeDefaultComponent().getAbsoluteRotation();
}

void MgeActor::setAbsoluteRotation(float rotation)
{
	editMgeDefaultComponent().setAbsoluteRotation(rotation);
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

void MgeActor::createMgeDefaultComponent()
{
	defaultActorData = std::make_shared<MgeDefaultComponent>();
	addComponent(defaultActorData);
}



