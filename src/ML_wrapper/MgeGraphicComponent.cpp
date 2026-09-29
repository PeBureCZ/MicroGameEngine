#include "MgeGraphicComponent.h"

MgeGraphicComponent::MgeGraphicComponent(MGE_GRAPHIC_VARIANT&& graphicObject)
	: MgeBasicComponent(MgePredefinedComponents::GRAPHIC)
{
	m_graphic.push_back(std::move(graphicObject));
}

MgeGraphicComponent::MgeGraphicComponent(std::string_view text, unsigned int characterSize_pxls)
{
	MgeText newText(std::move(std::string(text)), characterSize_pxls);
	m_graphic.push_back(std::move(newText));
}

void MgeGraphicComponent::addGraphicVariant(MGE_GRAPHIC_VARIANT&& graphicObject)
{
	m_graphic.push_back(std::move(graphicObject));
}

void MgeGraphicComponent::setVariant(MGE_GRAPHIC_VARIANT&& graphicObject, size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return;

	m_graphic[index] = std::move(graphicObject);
}

size_t MgeGraphicComponent::getComponentSize()
{
	return m_graphic.size();
}

void MgeGraphicComponent::setIsVisible(bool visible)
{
	for (auto& component : m_graphic)
	{
		if (std::holds_alternative<MgeDrawable>(component))
		{
			auto& drawableObject = std::get<MgeDrawable>(component);
			drawableObject.setIsVisible(visible);
		}
		else if (std::holds_alternative<MgeImage>(component))
		{
			auto& img = std::get<MgeImage>(component);
			img.setVisible(visible);
		}
		else if (std::holds_alternative<MgeText>(component))
		{
			auto& text = std::get<MgeText>(component);
			text.setIsVisible(visible);
		}
	}
}

std::optional<size_t> MgeGraphicComponent::getLayerFromVariant(size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return std::nullopt;

	if (std::holds_alternative<MgeDrawable>(m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(m_graphic[index]);
		return drawableObject.getLayer();
	}
	else if (std::holds_alternative<MgeImage>(m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(m_graphic[index]);
		return img.getLayer();

	}
	else if (std::holds_alternative<MgeText>(m_graphic[index]))
	{
		auto& text = std::get<MgeText>(m_graphic[index]);
		return text.getLayer();
	}

	_ASSERT(false);
	return std::nullopt;
}

void MgeGraphicComponent::setColor(const mgeType::Color_RGBA& newColor, size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return;

	if (std::holds_alternative<MgeDrawable>(m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(m_graphic[index]);
		drawableObject.setColor(newColor);
	}
	else if (std::holds_alternative<MgeImage>(m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(m_graphic[index]);
		img.setColor(newColor);
	}
	else if (std::holds_alternative<MgeText>(m_graphic[index]))
	{
		auto& text = std::get<MgeText>(m_graphic[index]);
		text.setColor(newColor);
	}
	else
	{ //unhandled
		_ASSERT(false);
	} 
}

void MgeGraphicComponent::setRotation(float rotation, size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return;

	if (std::holds_alternative<MgeDrawable>(m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(m_graphic[index]);
		drawableObject.setRotation(rotation);
	}
	else if (std::holds_alternative<MgeImage>(m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(m_graphic[index]);
		img.setRotation(rotation);
	}
	else if (std::holds_alternative<MgeText>(m_graphic[index]))
	{
		_ASSERT(false); //not yet
	}
	else
	{ //unhandled
		_ASSERT(false);
	}
}

void MgeGraphicComponent::setPosition(const FPoint& position, size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return;

	if (std::holds_alternative<MgeDrawable>(m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(m_graphic[index]);
		drawableObject.setPosition(position);
	}
	else if (std::holds_alternative<MgeImage>(m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(m_graphic[index]);
		img.setImgAbsolutePosition(position);
	}
	else if (std::holds_alternative<MgeText>(m_graphic[index]))
	{
		auto& text = std::get<MgeText>(m_graphic[index]);
		text.setAbsolutePosition(position.asInt());
	}
	else
	{ //unhandled
		_ASSERT(false);
	}
}

std::optional<FPoint> MgeGraphicComponent::getPosition(size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return std::nullopt;

	if (std::holds_alternative<MgeDrawable>(m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(m_graphic[index]);
		return drawableObject.getPosition();
	}
	else if (std::holds_alternative<MgeImage>(m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(m_graphic[index]);
		return img.getAbsolutePosition();
	}
	else if (std::holds_alternative<MgeText>(m_graphic[index]))
	{
		auto& text = std::get<MgeText>(m_graphic[index]);
		return text.getAbsolutePosition().asFloat();
	}
	else
	{ //unhandled
		_ASSERT(false);
	}

	return std::nullopt;
}

void MgeGraphicComponent::setOrigin(const FPoint& newOrigin, size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return;

	if (std::holds_alternative<MgeDrawable>(m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(m_graphic[index]);
		_ASSERT(false); //not yet
	}
	else if (std::holds_alternative<MgeImage>(m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(m_graphic[index]);
		img.setOrigin(newOrigin);
	}
	else if (std::holds_alternative<MgeText>(m_graphic[index]))
	{
		_ASSERT(false); //not yet
	}
	else
	{ //unhandled
		_ASSERT(false);
	}
}

std::optional<float> MgeGraphicComponent::getRotation(size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return std::nullopt;

	if (std::holds_alternative<MgeDrawable>(m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(m_graphic[index]);
		return drawableObject.getRotation();
	}
	else if (std::holds_alternative<MgeImage>(m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(m_graphic[index]);
		return img.getRotation();
	}
	else if (std::holds_alternative<MgeText>(m_graphic[index]))
	{
		_ASSERT(false); //not yet
	}
	else
	{ //unhandled
		_ASSERT(false);
	}
	return std::nullopt;
}



