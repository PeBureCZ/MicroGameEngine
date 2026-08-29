#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "Transform.h"

enum ComponentType : uint64_t
{
	UNDEFINED = 0,
	MGE_DEFAULT, //parent system
	TRANSFORM,
	GRAPHIC,
	//add new MGE types here...

	//last MGE component type. Do not assign values greater than this
	LAST_MGE_COMPONENT_TYPE_ENUM = 50000

	//...
	//...
	//...custom (user) elements...
	//...
	//...
};

class MgeBasicComponent
{
public:
	MgeBasicComponent(ComponentType type = ComponentType::UNDEFINED, std::string_view name = "")
		: m_name(name), m_type(type)
	{
	}

	void setName(std::string_view name) noexcept;
	[[nodiscard]] const std::string& getName() const noexcept;

	MgeBasicComponent(const MgeBasicComponent&) = default;
	MgeBasicComponent(MgeBasicComponent&&) = default;
	MgeBasicComponent& operator=(const MgeBasicComponent&) = default;
	MgeBasicComponent& operator=(MgeBasicComponent&&) = default;
	virtual ~MgeBasicComponent() = default;

	[[nodiscard]] ComponentType getType() const noexcept;

private:
	std::string m_name;
	ComponentType m_type = ComponentType::UNDEFINED;
};

class MgeTransform : public MgeBasicComponent
{
public:
	MgeTransform(ComponentType type = ComponentType::TRANSFORM, std::string_view name = "") : MgeBasicComponent(type, name)
	{

	}

	[[nodiscard]] const FPoint& getPosition() const noexcept;
	void setPosition(const FPoint& position) noexcept;

	[[nodiscard]] const FSize& getSize() const noexcept;
	void setSize(const FSize& size) noexcept;

	[[nodiscard]] float getRotation() const noexcept;
	void setRotation(float rotation);

private:
	BasicTransform m_transform;
};



