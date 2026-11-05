#include "MgeButton.h"

constexpr MgeColor DEFAULT_IMAGE_COLOR = MgeColor(255, 255, 255);

MgeButton::MgeButton(const FPoint& newPosition, mgeType::Size<int> newSize, GraphicItemLayer layer)
	: MgeFrame(newPosition, newSize, layer)
	
{
	setColor(defaultColor);
	setBasicCollision();
}

MgeButton::MgeButton(const FPoint& newPosition, MgeImage&& image, const TextureId& idHovered, const TextureId& idClicked, GraphicItemLayer layer)
	: m_hoveredTexture(idHovered), clickedTexture(idClicked), MgeFrame(newPosition, ISize(image.getSize()), layer)
{
	unselectedTexture = image.getTextureID();
	image.setImgAbsolutePosition(newPosition);
	defaultColor = DEFAULT_IMAGE_COLOR;
	mouseOverColor = DEFAULT_IMAGE_COLOR;
	MgeFrame::setImage(std::move(image));
}

void MgeButton::setDefaultButtonColor(const MgeColor& newColor)
{
	defaultColor = newColor;
	if (!isUnderCursor())
		setColor(defaultColor);
}

void MgeButton::setMouseOverButtonColor(const MgeColor& newColor)
{
	mouseOverColor = newColor;
	if (isUnderCursor()) 
		setColor(mouseOverColor);
}

void MgeButton::setIsVisible(bool visible) noexcept
{
	if (m_buttonText)
		m_buttonText->setIsVisible(visible);
	MgeFrame::setIsVisible(visible);
}

void MgeButton::setOnLMBClick(VoidCallback clickFunction) noexcept
{
	onLMBClick  = std::move(clickFunction);
}

void MgeButton::setOnRMBClick(VoidCallback clickFunction) noexcept
{
	onRMBClick = std::move(clickFunction);
}

void MgeButton::onLmbClickCall() noexcept
{
	try
	{
		if (onLMBClick)
			onLMBClick();
	}
	catch (...)
	{
		_ASSERT(false); //unexpected exception
	}
}

void MgeButton::onRmbClickCall() noexcept
{
	try
	{
		if (onRMBClick)
			onRMBClick();
	}
	catch (...)
	{
		_ASSERT(false); //unexpected exception
	}
}

void MgeButton::layout() noexcept
{
	try
	{
		if (m_buttonText)
			m_buttonText->setAbsolutePosition(getAlignedPosition(m_buttonText->getAlign(), m_buttonText->getTextSize()));
	}
	catch (...)
	{
		_ASSERT(false); //unexpected exception
	}

	MgeFrame::layout();
}

void MgeButton::addTextToButton(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align, const MgeColor& col, int64_t zPos)
{
	if (!m_buttonText)
	{
		size_t layer = GraphicItemLayer::GUI_LAYER;
		if (auto graphic = getGraphicComponent())
			layer = graphic->getLayerFromVariant(GraphicType::BASIC_GRAPHIC_INDEX).value_or(GraphicItemLayer::GUI_LAYER);

		m_buttonText = std::make_unique<MgeText>(butText, characterSize_pxls, FPoint(), false, layer);
		m_buttonText->setAlign(align);
		m_buttonText->setIsVisible(getIsVisible());
		m_buttonText->setZPosition(zPos);
		m_buttonText->setColor(col);
		m_buttonText->setAbsolutePosition(getAlignedPosition(align, m_buttonText->getTextSize()));
	}
	else
	{
		_ASSERT(false); //not yet
	}
}

void MgeButton::setButtonTextColors(MgeColor defaultColor, MgeColor mouseOverColor)
{
	m_defaultTextColor = defaultColor;
	m_mouseOverTextColor = mouseOverColor;
	if (m_buttonText)
	{
		if (m_isUnderMouseCursor)
			m_buttonText->setColor(m_mouseOverTextColor);
		else
			m_buttonText->setColor(m_defaultTextColor);
	}
}

void MgeButton::setButtonText(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align)
{
	int64_t zPos = m_buttonText ? m_buttonText->getZPosition() : mgeCore::getDefaultZPosition(mgeCore::ZPositionAdditionMode::Text);
	m_buttonText.reset();

	addTextToButton(butText, characterSize_pxls, align, m_isUnderMouseCursor ? m_mouseOverTextColor : m_defaultTextColor, zPos);
}

void MgeButton::setButtonText(const std::string& butText)
{
	if (m_buttonText)
	{
		m_buttonText->setText(butText);
		m_buttonText->setAbsolutePosition(getAlignedPosition(m_buttonText->getAlign(), m_buttonText->getTextSize()));
	}
	else
	{
		_ASSERT(false);
	}
}

std::string MgeButton::getButtonText() const
{
	if (m_buttonText)
		return m_buttonText->getText();
	else
	{
		_ASSERT(false);
		return {};
	}
}

MgeText& MgeButton::getButtonTextObject()
{
	if (!m_buttonText)
		addTextToButton("", 12, GuiAlign::MiddleCenter, MgeColor());
	return *m_buttonText;
}

void MgeButton::onCursorEnterCall() noexcept
{
	try
	{
		setColor(mouseOverColor);

		if (m_buttonText)
			m_buttonText->setColor(m_mouseOverTextColor);

		if (auto graphic = getGraphicComponent())
		{
			auto graphicVariant = graphic->getVariant(GraphicType::BASIC_GRAPHIC_INDEX);
			if (graphicVariant && std::holds_alternative<MgeImage>(*graphicVariant))
			{
				auto& img = std::get<MgeImage>(*graphicVariant);
				if (m_hoveredTexture != TextureId() && !m_hoveredTexture.getPath().empty())
					img.setTexture(m_hoveredTexture);
			}
		}

		MgeFrame::onCursorEnterCall();
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

void MgeButton::onCursorLeaveCall() noexcept
{
	try
	{
		setColor(defaultColor);
			
		if (m_buttonText)
			m_buttonText->setColor(m_defaultTextColor);

		if (auto graphic = getGraphicComponent())
		{
			auto graphicVariant = graphic->getVariant(GraphicType::BASIC_GRAPHIC_INDEX);
			if (graphicVariant && std::holds_alternative<MgeImage>(*graphicVariant))
			{
				auto& img = std::get<MgeImage>(*graphicVariant);
				if (unselectedTexture != TextureId() && !unselectedTexture.getPath().empty())
					img.setTexture(unselectedTexture);
			}
		}

		MgeFrame::onCursorLeaveCall();
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

namespace mge
{
	Button mge::createButton(const FPoint& position, const ISize& size, GraphicItemLayer layer)
	{
		auto newButton = std::make_shared<MgeButton>(position, size, layer);
		newButton->initializeSelf(newButton);
		return newButton;
	}

	Button createButton(const FPoint& position, MgeImage&& image, const TextureId& idHovered, const TextureId& idClicked, GraphicItemLayer layer)
	{
		auto newButton = std::make_shared<MgeButton>(position, std::move(image), idHovered, idClicked, layer);
		newButton->initializeSelf(newButton);
		return newButton;
	}
}

