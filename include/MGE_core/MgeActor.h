#pragma once
#include "MgeObject.h"
#include <memory>
#include <array>
#include <vector>

#include "MgeComponents.h"
#include "BasicTypes.h"
#include "EventSystem.h"

class MgeActor;

class MgeDefaultComponent : public MgeTransform
{
public:
	MgeDefaultComponent() : MgeTransform(ComponentType::MGE_DEFAULT)
	{

	}

	MgeDefaultComponent(const FPoint& pos, const std::shared_ptr<MgeActor>& parent = std::shared_ptr<MgeActor>())
		: MgeTransform(ComponentType::MGE_DEFAULT)
	{

	}

	void addChild(const std::shared_ptr<MgeActor>& child) noexcept;
	void setParent(const std::shared_ptr<MgeActor>& newParent = std::shared_ptr<MgeActor>()) noexcept;

	[[nodiscard]] const std::vector<std::shared_ptr<MgeActor>>& getChildren() const noexcept;
	[[nodiscard]] std::vector<std::shared_ptr<MgeActor>>& editChildren() noexcept;

	[[nodiscard]] std::optional<std::shared_ptr<MgeActor>> editParent() noexcept;
	[[nodiscard]] std::optional<const std::shared_ptr<MgeActor>> getParent() const noexcept;

	[[nodiscard]] bool removeChild(std::shared_ptr<MgeActor>& child);
	[[nodiscard]] bool removeChild(MgeObjectId childId);

	[[nodiscard]] FPoint getAbsolutePosition() const noexcept;
	[[nodiscard]] const FPoint& getPosition() const noexcept;
	void setAbsolutePosition(const FPoint& position) noexcept;
	void setRelativePosition(const FPoint& position) noexcept;

	[[nodiscard]] float getAbsoluteRotation() const noexcept;
	[[nodiscard]] float getRelativeRotation() const noexcept;
	void setAbsoluteRotation(float rotation) noexcept;
	void setRelativeRotation(float rotation) noexcept;

private:

	std::weak_ptr<MgeActor> parent;
	std::vector<std::shared_ptr<MgeActor>> m_children;
};

class MgeBasicActor : public MgeObject
{
public:
	MgeBasicActor() = default;

	const std::vector<std::shared_ptr<MgeBasicComponent>>& getComponents() const noexcept;
	std::vector<std::shared_ptr<MgeBasicComponent>>& editComponents() noexcept;
	std::optional<std::shared_ptr<MgeBasicComponent>> editComponent(ComponentType type) noexcept;

	void addComponent(std::shared_ptr<MgeBasicComponent> newComponent);

private:
	std::vector<std::shared_ptr<MgeBasicComponent>> m_components;
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
	[[nodiscard]] std::optional<const std::shared_ptr<MgeActor>> getParent() const noexcept;
	[[nodiscard]] const std::vector<std::shared_ptr<MgeActor>>& getChildren() const noexcept;
	[[nodiscard]] std::vector<std::shared_ptr<MgeActor>>& editChildren() noexcept;
	void addChild(const std::shared_ptr<MgeActor>& child) noexcept;
	[[nodiscard]] bool removeChild(std::shared_ptr<MgeActor>& child);
	[[nodiscard]] bool removeChild(MgeObjectId childId);

	MgeDefaultComponent& editMgeDefaultComponent() noexcept;
	const MgeDefaultComponent& getMgeDefaultComponent() const noexcept;

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

