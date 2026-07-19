#pragma once
#include "MgeObject.h"
#include <memory>
#include <array>
#include <vector>
#include "MgeComponents.h"
#include "BasicTypes.h"

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
	[[nodiscard]] const FPoint& getRelativePosition() const noexcept;

	void setAbsolutePosition(const FPoint& position) noexcept;
	void setRelativePosition(const FPoint& position) noexcept;

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

private:
	std::vector<std::shared_ptr<MgeBasicComponent>> m_components;
};

class MgeActor : public MgeBasicActor
{
public:
	MgeActor(FPoint pos, std::shared_ptr<MgeActor> parent = std::shared_ptr<MgeActor>())
	{
		defaultActorData.setRelativePosition(pos);
		defaultActorData.setParent(parent);
	}

	void setRelativePosition(const FPoint& newPosition) noexcept;
	void setAbsolutePosition(const FPoint& newPosition) noexcept;

	[[nodiscard]] const FPoint& getRelativePosition() const noexcept;
	[[nodiscard]] FPoint getAbsolutePosition() const noexcept;

	MgeDefaultComponent& editMgeDefaultComponent() noexcept;
	const MgeDefaultComponent& getMgeDefaultComponent() const noexcept;

private:
	MgeDefaultComponent defaultActorData;
};

