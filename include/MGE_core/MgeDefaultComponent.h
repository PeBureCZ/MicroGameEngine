#pragma once

#include "MgeComponents.h"

#include <memory>
#include <optional>

#include "MgeObject.h"

class MgeActor;

class MgeDefaultComponent : public MgeTransform
{
public:
	MgeDefaultComponent() : MgeTransform(MgePredefinedComponents::MGE_DEFAULT)
	{}

	MgeDefaultComponent(const FPoint& pos, const std::shared_ptr<MgeActor>& parent = std::shared_ptr<MgeActor>())
		: MgeTransform(MgePredefinedComponents::MGE_DEFAULT)
	{}

	void addChild(const std::shared_ptr<MgeActor>& child, std::optional<size_t> toIndex = std::nullopt) noexcept;
	void setParent(const std::shared_ptr<MgeActor>& newParent = std::shared_ptr<MgeActor>()) noexcept;

	[[nodiscard]] const std::vector<std::shared_ptr<MgeActor>>& getChildren() const noexcept;
	[[nodiscard]] std::vector<std::shared_ptr<MgeActor>>& editChildren() noexcept;

	[[nodiscard]] std::shared_ptr<MgeActor> editParent() noexcept;
	[[nodiscard]] const std::shared_ptr<MgeActor> getParent() const noexcept;

	//get the last parent in the queue of parents
	[[nodiscard]] const std::shared_ptr<MgeActor> getMasterParent() const noexcept;

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