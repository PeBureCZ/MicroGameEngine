#include "MgeGraphicComponent.h"

MgeGraphicComponent::MgeGraphicComponent(const MGE_GRAPHIC_PTR& graphicObject)
	: MgeBasicComponent(MgePredefinedComponents::GRAPHIC)
{
	m_graphic.push_back(graphicObject);
}

MgeGraphicComponent::MgeGraphicComponent(MgeImage&& graphicObject)
	: MgeBasicComponent(MgePredefinedComponents::GRAPHIC)
{
	auto newImageGraphic = std::make_shared<MGE_GRAPHIC_VARIANT>(std::move(graphicObject));
	m_graphic.push_back(newImageGraphic);
}

MgeGraphicComponent::MgeGraphicComponent(MgeDrawable&& graphicObject)
	: MgeBasicComponent(MgePredefinedComponents::GRAPHIC)
{
	auto newDrawableGraphic = std::make_shared<MGE_GRAPHIC_VARIANT>(std::move(graphicObject));
	m_graphic.push_back(newDrawableGraphic);
}

MgeGraphicComponent::MgeGraphicComponent(MgeText&& graphicObject)
	: MgeBasicComponent(MgePredefinedComponents::GRAPHIC)
{
	auto newTextGraphic = std::make_shared<MGE_GRAPHIC_VARIANT>(std::move(graphicObject));
	m_graphic.push_back(newTextGraphic);
}

MgeGraphicComponent::MgeGraphicComponent(std::string_view text, unsigned int characterSize_pxls)
	: MgeBasicComponent(MgePredefinedComponents::GRAPHIC)
{
	auto newTextGraphic = std::make_shared<MGE_GRAPHIC_VARIANT>(MgeText(std::move(std::string(text)), characterSize_pxls));
	m_graphic.push_back(newTextGraphic);
}

void MgeGraphicComponent::addGraphicVariant(const MGE_GRAPHIC_PTR& graphicObject)
{
	m_graphic.push_back(graphicObject);
}

void MgeGraphicComponent::setVariant(const MGE_GRAPHIC_PTR& graphicObject, size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return;

	m_graphic[index] = graphicObject;
}

size_t MgeGraphicComponent::getComponentSize()
{
	return m_graphic.size();
}

void MgeGraphicComponent::setIsVisible(bool visible)
{
	for (auto& component : m_graphic)
	{
		_ASSERT(component);
		if (!component)
			continue;

		if (std::holds_alternative<MgeDrawable>(*component))
		{
			auto& drawableObject = std::get<MgeDrawable>(*component);
			drawableObject.setIsVisible(visible);
		}
		else if (std::holds_alternative<MgeImage>(*component))
		{
			auto& img = std::get<MgeImage>(*component);
			img.setVisible(visible);
		}
		else if (std::holds_alternative<MgeText>(*component))
		{
			auto& text = std::get<MgeText>(*component);
			text.setIsVisible(visible);
		}
	}
}

std::optional<size_t> MgeGraphicComponent::getLayerFromVariant(size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return std::nullopt;

	_ASSERT(m_graphic[index]);
	if (!m_graphic[index])
		return std::nullopt;

	if (std::holds_alternative<MgeDrawable>(*m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(*m_graphic[index]);
		return drawableObject.getLayer();
	}
	else if (std::holds_alternative<MgeImage>(*m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(*m_graphic[index]);
		return img.getLayer();

	}
	else if (std::holds_alternative<MgeText>(*m_graphic[index]))
	{
		auto& text = std::get<MgeText>(*m_graphic[index]);
		return text.getLayer();
	}

	_ASSERT(false);
	return std::nullopt;
}

void MgeGraphicComponent::setColor(const MgeColor& newColor, size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return;

	_ASSERT(m_graphic[index]);
	if (!m_graphic[index])
		return;

	if (std::holds_alternative<MgeDrawable>(*m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(*m_graphic[index]);
		drawableObject.setColor(newColor);
	}
	else if (std::holds_alternative<MgeImage>(*m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(*m_graphic[index]);
		img.setColor(newColor);
	}
	else if (std::holds_alternative<MgeText>(*m_graphic[index]))
	{
		auto& text = std::get<MgeText>(*m_graphic[index]);
		text.setColor(newColor);
	}
	else
	{ //unhandled
		_ASSERT(false);
	} 
}

void MgeGraphicComponent::rescaleGraphic(float scaleX, float scaleY, size_t index)
{
	_ASSERT(index < m_graphic.size());
	if (index >= m_graphic.size())
		return;

	_ASSERT(m_graphic[index]);
	if (!m_graphic[index])
		return;

	if (std::holds_alternative<MgeDrawable>(*m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(*m_graphic[index]);
		drawableObject.rescale(scaleX, scaleY);
	}
	else if (std::holds_alternative<MgeImage>(*m_graphic[index]))
	{
		_ASSERT(false); //not yet
	}
	else if (std::holds_alternative<MgeText>(*m_graphic[index]))
	{
		_ASSERT(false); //not yet
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

	_ASSERT(m_graphic[index]);
	if (!m_graphic[index])
		return;

	if (std::holds_alternative<MgeDrawable>(*m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(*m_graphic[index]);
		drawableObject.setRotation(rotation);
	}
	else if (std::holds_alternative<MgeImage>(*m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(*m_graphic[index]);
		img.setRotation(rotation);
	}
	else if (std::holds_alternative<MgeText>(*m_graphic[index]))
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

	_ASSERT(m_graphic[index]);
	if (!m_graphic[index])
		return;

	if (std::holds_alternative<MgeDrawable>(*m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(*m_graphic[index]);
		drawableObject.setPosition(position);
	}
	else if (std::holds_alternative<MgeImage>(*m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(*m_graphic[index]);
		img.setImgAbsolutePosition(position);
	}
	else if (std::holds_alternative<MgeText>(*m_graphic[index]))
	{
		auto& text = std::get<MgeText>(*m_graphic[index]);
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

	_ASSERT(m_graphic[index]);
	if (!m_graphic[index])
		return std::nullopt;

	if (std::holds_alternative<MgeDrawable>(*m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(*m_graphic[index]);
		return drawableObject.getPosition();
	}
	else if (std::holds_alternative<MgeImage>(*m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(*m_graphic[index]);
		return img.getAbsolutePosition();
	}
	else if (std::holds_alternative<MgeText>(*m_graphic[index]))
	{
		auto& text = std::get<MgeText>(*m_graphic[index]);
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

	_ASSERT(m_graphic[index]);
	if (!m_graphic[index])
		return;

	if (std::holds_alternative<MgeDrawable>(*m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(*m_graphic[index]);
		_ASSERT(false); //not yet
	}
	else if (std::holds_alternative<MgeImage>(*m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(*m_graphic[index]);
		img.setOrigin(newOrigin);
	}
	else if (std::holds_alternative<MgeText>(*m_graphic[index]))
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

	_ASSERT(m_graphic[index]);
	if (!m_graphic[index])
		return std::nullopt;

	if (std::holds_alternative<MgeDrawable>(*m_graphic[index]))
	{
		auto& drawableObject = std::get<MgeDrawable>(*m_graphic[index]);
		return drawableObject.getRotation();
	}
	else if (std::holds_alternative<MgeImage>(*m_graphic[index]))
	{
		auto& img = std::get<MgeImage>(*m_graphic[index]);
		return img.getRotation();
	}
	else if (std::holds_alternative<MgeText>(*m_graphic[index]))
	{
		_ASSERT(false); //not yet
	}
	else
	{ //unhandled
		_ASSERT(false);
	}
	return std::nullopt;
}


MgeGraphicComponent::~MgeGraphicComponent()
{
#ifdef _DEBUG
	for (auto& component : m_graphic)
	{
		_ASSERT(component);
		if (!component)
			continue;

		//only this component should have the ownership of the graphic object during destruction
		//if not, it means that the graphic object is still used somewhere else, which is not proper
		// and can lead to undefined behavior
		_ASSERT(component.use_count() == 1); 
	}
#endif //_DEBUG
	m_graphic.clear();
}
