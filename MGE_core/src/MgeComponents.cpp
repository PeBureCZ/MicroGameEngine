#include "MgeComponents.h"

void MgeBasicComponent::setName(std::string_view name) noexcept
{
	m_name = name;
}

[[nodiscard]] const std::string& MgeBasicComponent::getName() const noexcept
{
	return m_name;
}

[[nodiscard]] ComponentType MgeBasicComponent::getType() const noexcept
{
	return m_type;
}


[[nodiscard]] const FPoint& MgeTransform::getPosition() const noexcept
{
	return m_transform.getPosition();
}

void MgeTransform::setPosition(const FPoint& position) noexcept
{
	m_transform.setPosition(position);
}


[[nodiscard]] const FSize& MgeTransform::getSize() const noexcept
{
	return m_transform.getSize();
}

void MgeTransform::setSize(const FSize& size) noexcept
{
	m_transform.setSize(size);
}
