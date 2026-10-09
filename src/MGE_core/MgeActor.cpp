#include "MgeActor.h"

#include <algorithm>

constexpr size_t USE_BINARY_SEARCH_WITH_SIZE = 16;

const std::vector<MGE_COMPONENT>& MgeBasicActor::getComponents() const noexcept
{
	return m_components;
}

std::vector<MGE_COMPONENT>& MgeBasicActor::editComponents() noexcept
{
	return m_components;
}

MGE_COMPONENT MgeBasicActor::editComponent(uint64_t type) noexcept
{
	const auto& components = editComponents();

	if (getComponentCount() < USE_BINARY_SEARCH_WITH_SIZE)
	{
		for (auto& component : components)
		{
			if (component->getType() == type)
				return component;
		}
	}
	else
	{
		auto it = std::lower_bound(components.begin(), components.end(), type, [](const MGE_COMPONENT component, uint64_t type_id)
			{ return component->getType() == type_id; });

		if (it == components.end())
		{
			_ASSERT(false);
			return nullptr;
		}
		return *it;
	}
	return nullptr;
}

void MgeBasicActor::addComponent(MGE_COMPONENT newComponent)
{
	_ASSERT(newComponent);
	if (!newComponent)
		return;

	if (getComponentCount() + 1 == USE_BINARY_SEARCH_WITH_SIZE)
	{ //sort the vector before adding the new component
		m_components.push_back(newComponent);
		std::sort(m_components.begin(), m_components.end(), [](const MGE_COMPONENT component1, const MGE_COMPONENT component2)
			{ return component1->getType() < component2->getType(); });
	}
	else if (getComponentCount() + 1 < USE_BINARY_SEARCH_WITH_SIZE)
		m_components.push_back(newComponent);
	else
	{
		auto it = std::lower_bound(m_components.begin(), m_components.end(), newComponent->getType(), [](const MGE_COMPONENT component, uint64_t type_id)
			{ return component->getType() < type_id;  });
		m_components.insert(it, newComponent);
	}

#ifdef _DEBUG
	int64_t counter = -1;
	if (getComponentCount() >= USE_BINARY_SEARCH_WITH_SIZE)
	{
		for (auto& component : editComponents())
		{
			_ASSERT((int64_t)component->getType() > counter);
			counter = component->getType();
		}
	}
#endif
}

//##############		MgeActor			##############

void MgeActor::setRelativePosition(const FPoint& newPosition) noexcept
{
#ifdef _DEBUG
	[[maybe_unused]] auto id = getId();
	if (id > 0)
		id = id;
#endif
	editMgeDefaultComponent().setRelativePosition(newPosition + m_relativeOffset);
}

void MgeActor::setAbsolutePosition(const FPoint& newPosition) noexcept
{
#ifdef _DEBUG
	[[maybe_unused]] auto id = getId();
	if (id > 0)
		id = id;
#endif
	editMgeDefaultComponent().setAbsolutePosition(newPosition);
}

void MgeActor::setParent(const std::shared_ptr<MgeActor>& newParent) noexcept
{
	editMgeDefaultComponent().setParent(newParent);
}

[[nodiscard]] const std::shared_ptr<MgeActor> MgeActor::getParent() const noexcept
{
	return getMgeDefaultComponent().getParent();
}

[[nodiscard]] const std::shared_ptr<MgeActor> MgeActor::getMasterParent() const noexcept
{
	return getMgeDefaultComponent().getMasterParent();
}

[[nodiscard]] const std::vector<std::shared_ptr<MgeActor>>& MgeActor::getChildren() const noexcept
{
	return getMgeDefaultComponent().getChildren();
}

std::vector<std::shared_ptr<MgeActor>>& MgeActor::editChildren() noexcept
{
	return editMgeDefaultComponent().editChildren();
}

void MgeActor::addChild(const std::shared_ptr<MgeActor>& child, std::optional<size_t> toIndex) noexcept
{
	editMgeDefaultComponent().addChild(child, toIndex);
}

[[nodiscard]] bool MgeActor::removeChild(std::shared_ptr<MgeActor>& child)
{
	return editMgeDefaultComponent().removeChild(child);
}

[[nodiscard]] bool MgeActor::removeChild(MgeObjectId childId)
{
	return editMgeDefaultComponent().removeChild(childId);
}

[[nodiscard]] FPoint MgeActor::getRelativePosition() const noexcept
{
	return getMgeDefaultComponent().getPosition() - m_relativeOffset;
}

[[nodiscard]] FPoint MgeActor::getAbsolutePosition() const noexcept
{
	return getMgeDefaultComponent().getAbsolutePosition();
}

void MgeActor::setRelativeRotation(float rotation)
{
	return editMgeDefaultComponent().setRotation(rotation);
}

void MgeActor::setPositionOffset(const FPoint& offset) noexcept
{
	auto dif = offset - m_relativeOffset;
	setRelativePosition(getRelativePosition() + dif);
	m_relativeOffset = offset;
}

const FPoint& MgeActor::getPositionOffset() const noexcept
{
	return m_relativeOffset;
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

void MgeActor::createMgeDefaultComponent()
{
	defaultActorData = std::make_shared<MgeDefaultComponent>();
	addComponent(defaultActorData);
}

namespace mge
{
	bool destroyActor(std::shared_ptr<MgeActor> actor)
	{
		bool removed = false;
		MAIN_THREAD_GUARD; //must be done in MT due to event system
		_ASSERT(actor);
		if (!actor)
			return removed;

		for (auto& child : actor->editChildren())
		{
			_ASSERT(child);
			if (child)
				destroyActor(child);
		}

		if (auto parent = actor->getParent())
		{
			MgeObjectId id = actor->getId();
			actor.reset();
			removed = parent->removeChild(id); //remove self in parent vector (owner)
			_ASSERT(removed);
		}
		return removed;
	}
}