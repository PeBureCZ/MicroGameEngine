#include "MgeButton.h"

constexpr MgeColor DEFAULT_IMAGE_COLOR = MgeColor(255, 255, 255);
constexpr MgeColor DISABLED_IMAGE_COLOR = MgeColor(255, 255, 255, 128); //semi-transparent

MgeButton::MgeButton(const FPoint& position, mgeType::Size<int> newSize, GraphicItemLayer layer)
	: MgeFrame(position, newSize, layer)
	
{
	MgeFrame::setColor(m_defaultColor);
	setBasicCollision();
}

MgeButton::MgeButton(const FPoint& position, MgeImage&& imgWithDefaultTexture, const TextureId& hovered,
	const TextureId& active, const TextureId& disabled, const TextureId& activeHovered, GraphicItemLayer layer)
	: MgeFrame(position, ISize(imgWithDefaultTexture.getSize()), layer)
{
	m_defaultTexture = imgWithDefaultTexture.getTextureID();
	imgWithDefaultTexture.setImgAbsolutePosition(position);
	m_hoveredTexture = hovered;
	m_disabledTexture = disabled;
	m_activeTexture = active;
	m_hoveredActiveTexture = activeHovered;

	m_defaultColor = DEFAULT_IMAGE_COLOR;
	m_hoveredColor = DEFAULT_IMAGE_COLOR;
	m_disabledColor = DISABLED_IMAGE_COLOR;
	m_activeColor = DEFAULT_IMAGE_COLOR;
	m_hoveredActiveColor = DEFAULT_IMAGE_COLOR;
	MgeFrame::setImage(std::move(imgWithDefaultTexture));
}

void MgeButton::setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	MgeButton::setColor(MgeColor(r, g, b, a));
}

void MgeButton::setColor(const MgeColor& color) noexcept
{
	m_defaultColor = color;
	if (m_state == ButtonState::Normal && !isUnderCursor())
		MgeFrame::setColor(m_defaultColor);
}

void MgeButton::setHoveredColor(const MgeColor& color) noexcept
{
	m_hoveredColor = color;
	if (m_state == ButtonState::Normal && isUnderCursor())
		MgeFrame::setColor(m_hoveredColor);
}

void MgeButton::setDisabledColor(const MgeColor& color) noexcept
{
	m_disabledColor = color;
	if (getIsDisabled())
		MgeFrame::setColor(m_disabledColor);
}

void MgeButton::setActiveColor(const MgeColor& color) noexcept
{
	m_activeColor = color;
	if (m_state == ButtonState::Active && !isUnderCursor())
		MgeFrame::setColor(m_activeColor);
}

void MgeButton::setHoveredActiveColor(const MgeColor& color) noexcept
{
	m_hoveredActiveColor = color;
	if (m_state == ButtonState::Active && isUnderCursor())
		MgeFrame::setColor(m_hoveredActiveColor);
}

void MgeButton::setDisabledTexture(const TextureId& textureId)
{
	m_disabledTexture = textureId;
	if (getIsDisabled())
		applyLook();
}

void MgeButton::setState(ButtonState newState) noexcept
{
	if (m_state == newState)
		return;

	m_state = newState;
	applyLook();
}

ButtonState MgeButton::getState() const noexcept
{
	return m_state;
}

void MgeButton::setIsDisabled(bool disabled) noexcept
{
	if (disabled)
		setState(getIsActive() ? ButtonState::DisabledActive : ButtonState::Disabled);
	else
		setState(getIsActive() ? ButtonState::Active : ButtonState::Normal);
}

bool MgeButton::getIsDisabled() const noexcept
{
	return m_state == ButtonState::Disabled || m_state == ButtonState::DisabledActive;
}

void MgeButton::setIsActive(bool active) noexcept
{
	if (getIsDisabled())
		setState(active ? ButtonState::DisabledActive : ButtonState::Disabled);
	else
		setState(active ? ButtonState::Active : ButtonState::Normal);
}

bool MgeButton::getIsActive() const noexcept
{
	return m_state == ButtonState::Active || m_state == ButtonState::DisabledActive;
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
	if (getIsDisabled())
		return;

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
	if (getIsDisabled())
		return;

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
		m_buttonText->setDrawGroup(getDrawGroup());
		m_buttonText->setColor(col);
		m_buttonText->setAbsolutePosition(getAlignedPosition(align, m_buttonText->getTextSize()));
	}
	else
	{
		_ASSERT(false); //not yet
	}
}

void MgeButton::setButtonTextColors(const MgeColor& defaultColor, const MgeColor& hoveredColor, const MgeColor& disableColor,
	const MgeColor& activeColor, const MgeColor& hoveredActiveColor)
{
	m_defaultTextColor = defaultColor;
	m_hoveredTextColor = hoveredColor;
	m_disabledTextColor = disableColor;
	m_activeTextColor = activeColor;
	m_hoveredActiveTextColor = hoveredActiveColor;

	if (m_buttonText)
		m_buttonText->setColor(getTextColor());
}

void MgeButton::setButtonColors(const MgeColor& defaultColor, const MgeColor& hoveredColor, const MgeColor& disableColor,
	const MgeColor& activeColor, const MgeColor& hoveredActiveColor)
{
	m_defaultColor = defaultColor;
	m_hoveredColor = hoveredColor;
	m_disabledColor = disableColor;
	m_activeColor = activeColor;
	m_hoveredActiveColor = hoveredActiveColor;
	applyLook();
}

void MgeButton::setButtonText(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align)
{
	int64_t zPos = m_buttonText ? m_buttonText->getZPosition() : mgeCore::getDefaultZPosition(mgeCore::ZPositionAdditionMode::Text);
	m_buttonText.reset();

	addTextToButton(butText, characterSize_pxls, align, getTextColor(), zPos);
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
		addTextToButton("", 12, GuiAlign::MiddleCenter, getTextColor());
	return *m_buttonText;
}

void MgeButton::onCursorEnterCall() noexcept
{
	if (getIsDisabled())
		return; //no hover look, no enter callback

	applyLook();
	MgeFrame::onCursorEnterCall();
}

void MgeButton::onCursorLeaveCall() noexcept
{
	applyLook();
	MgeFrame::onCursorLeaveCall(); //also when disabled: the enter callback may have run before the button was disabled
}

void MgeButton::applyLook() noexcept
{
	auto setLook = [&](const MgeColor& color, const TextureId& texture)
		{
			MgeFrame::setColor(color);
			if (!texture.getPath().empty())
				setImageTexture(texture);
			else
			{
#ifdef _DEBUG //if the button uses images, texture should be valid
				auto graphic = getGraphicComponent();
				auto graphicVariant = graphic->getVariant(GraphicType::BASIC_GRAPHIC_INDEX);
				if (graphicVariant && std::holds_alternative<MgeImage>(*graphicVariant))
				{
					_ASSERT(false); //unhandled texture state
				}
#endif
				setImageTexture(m_defaultTexture);
			}
		};

	try
	{
		const bool isHovered = isUnderCursor();
		switch (m_state)
		{
		case ButtonState::Normal:
			setLook(isHovered ? m_hoveredColor : m_defaultColor, isHovered ? m_hoveredTexture : m_defaultTexture);
			break;
		case ButtonState::Active:
			setLook(isHovered ? m_hoveredActiveColor : m_activeColor, isHovered ? m_hoveredActiveTexture : m_activeTexture);
			break;
		case ButtonState::Disabled:
		case ButtonState::DisabledActive:
			setLook(m_disabledColor, m_disabledTexture);
			break;
		default:
			_ASSERT(false); //unhandled state
			break;
		}

		if (m_buttonText)
			m_buttonText->setColor(getTextColor());
	}
	catch (...)
	{
		_ASSERT(false); //unexpected exception
	}
}

void MgeButton::setImageTexture(const TextureId& textureId)
{
	if (textureId == TextureId() || textureId.getPath().empty())
		return;

	if (auto graphic = getGraphicComponent())
	{
		auto graphicVariant = graphic->getVariant(GraphicType::BASIC_GRAPHIC_INDEX);
		if (graphicVariant && std::holds_alternative<MgeImage>(*graphicVariant))
			std::get<MgeImage>(*graphicVariant).setTexture(textureId);
	}
}

const MgeColor& MgeButton::getTextColor() const noexcept
{
	switch (m_state)
	{
	case ButtonState::Active:
		return isUnderCursor() ? m_hoveredActiveTextColor : m_activeTextColor;
	case ButtonState::Disabled:
	case ButtonState::DisabledActive:
		return m_disabledTextColor;
	case ButtonState::Normal:
	default:
		return isUnderCursor() ? m_hoveredTextColor : m_defaultTextColor;
	}
}

void MgeButton::applyDrawGroupToGraphics() noexcept
{
	MgeFrame::applyDrawGroupToGraphics();
	if (m_buttonText && m_buttonText->isValid())
		m_buttonText->setDrawGroup(getDrawGroup());
}

namespace mge
{
	Button mge::createButton(const FPoint& position, const ISize& size, GraphicItemLayer layer)
	{
		auto newButton = std::make_shared<MgeButton>(position, size, layer);
		newButton->initializeSelf(newButton);
		return newButton;
	}

	Button createButton(const FPoint& position, MgeImage&& imgWithDefaultTexture, const TextureId& hovered,
		const TextureId& active, const TextureId& disabled, const TextureId& activeHovered, GraphicItemLayer layer)
	{
		auto newButton = std::make_shared<MgeButton>(position, std::move(imgWithDefaultTexture), hovered, active, disabled, activeHovered, layer);
		newButton->initializeSelf(newButton);
		return newButton;
	}
}

