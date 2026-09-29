#pragma once
#include "MgeObject.h"
#include <memory>
#include <array>
#include <vector>

#include "MgeDefaultComponent.h"
#include "BasicTypes.h"
#include "EventSystem.h"

class MgeBasicActor : public MgeObject
{
public:
	MgeBasicActor() = default;

	const std::vector<MGE_COMPONENT>& getComponents() const noexcept;
	MGE_COMPONENT editComponent(uint64_t type) noexcept;

	void addComponent(MGE_COMPONENT newComponent);
protected:
	[[nodiscard]] size_t getComponentCount() const noexcept { return m_components.size(); }
private:
	std::vector<MGE_COMPONENT> m_components;

	std::vector<MGE_COMPONENT>& editComponents() noexcept;
};

class MgeActor : public MgeBasicActor
{
public:
	MgeActor(FPoint pos, std::shared_ptr<MgeActor> parent = std::shared_ptr<MgeActor>())
	{
		createMgeDefaultComponent();

		setRelativePosition(pos);
		setParent(parent);
	}

	void setRelativePosition(const FPoint& newPosition) noexcept;
	void setAbsolutePosition(const FPoint& newPosition) noexcept;
	[[nodiscard]] FPoint getRelativePosition() const noexcept;
	[[nodiscard]] FPoint getAbsolutePosition() const noexcept;

	void setAbsoluteRotation(float rotation);
	void setRelativeRotation(float rotation);
	void setPositionOffset(const FPoint& offset) noexcept;
	[[nodiscard]] const FPoint& getPositionOffset() const noexcept;

	[[nodiscard]] float getRelativeRotation() const noexcept;
	[[nodiscard]] float getAbsoluteRotation() const noexcept;

	void setParent(const std::shared_ptr<MgeActor>& newParent = std::shared_ptr<MgeActor>()) noexcept;
	[[nodiscard]] const std::shared_ptr<MgeActor> getParent() const noexcept;

	//get the last parent in the queue of parents
	[[nodiscard]] const std::shared_ptr<MgeActor> getMasterParent() const noexcept;

	[[nodiscard]] const std::vector<std::shared_ptr<MgeActor>>& getChildren() const noexcept;
	[[nodiscard]] std::vector<std::shared_ptr<MgeActor>>& editChildren() noexcept;
	void addChild(const std::shared_ptr<MgeActor>& child) noexcept;
	[[nodiscard]] bool removeChild(std::shared_ptr<MgeActor>& child);
	[[nodiscard]] bool removeChild(MgeObjectId childId);

	MgeDefaultComponent& editMgeDefaultComponent() noexcept;
	const MgeDefaultComponent& getMgeDefaultComponent() const noexcept;

	virtual void destroy();

protected:

	template <typename Event>
	void sendEvent(Event&& ev)
	{
		getEventSystem().pushEvent(std::move(ev));
	}

	template <typename Event, typename Function>
	void bindEvent(Function&& fce)
	{
		eventsLifeTimeObserver.addToken<Event>(std::forward<Function>(fce));
	}

private:
	void createMgeDefaultComponent();

	ObserverTokens eventsLifeTimeObserver;
	FPoint m_relativeOffset;

	std::shared_ptr<MgeDefaultComponent> defaultActorData;
};

