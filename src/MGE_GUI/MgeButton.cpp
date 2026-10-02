#include "MgeButton.h"

MgeButton::MgeButton(const FPoint& newPosition, mgeType::Size<int> newSize, GraphicItemLayer layer)
	: MgeFrame(newPosition, newSize, layer)
	
{
	setColor(defaultColor);
	setBasicCollision();
}

MgeButton::MgeButton
	(
		const FPoint& newPosition,
		const TextureId& idUnselected,
		const TextureId& idSelected,
		const TextureId& idClicked
	)
	: MgeFrame(newPosition, idUnselected)
	, unselectedTexture(idUnselected), selectedTexture(idSelected), clickedTexture(idClicked)
{
	setBasicCollision();
}

MgeButton::MgeButton(const FPoint& newPosition, MgeImage&& image)
	: MgeFrame(newPosition, ISize(image.getSize()))
{
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
	if (buttonText)
		buttonText->setIsVisible(visible);
	MgeFrame::setIsVisible(visible);
}

void MgeButton::setOnLMBClick(Callback_deprecated clickFunction) noexcept
{
	onLMBClick  = std::move(clickFunction);
}

void MgeButton::setOnRMBClick(Callback_deprecated clickFunction) noexcept
{
	onRMBClick = std::move(clickFunction);
}

void MgeButton::onLmbClickCall() noexcept
{
	if (onLMBClick)
		onLMBClick();
}

void MgeButton::onRmbClickCall() noexcept
{
	if (onRMBClick)
		onRMBClick();
}

void MgeButton::layout() noexcept
{
	if (buttonText)
	{
		auto differencePos = getAbsolutePosition() - lastLayoutAbsolutePosition;
		const auto sizeChanged = (lastLayoutSize != getSize());

		if (sizeChanged)
			buttonText->setAbsolutePosition(getAlignedPosition(buttonText->getAlign(), buttonText->getTextSize()));
		else
			buttonText->setAbsolutePosition(buttonText->getAbsolutePosition() + differencePos.asInt());
	}

	MgeFrame::layout();
}

void MgeButton::addTextToButton(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align, const MgeColor& col)
{
	if (!buttonText)
	{
		size_t layer = GraphicItemLayer::GUI_LAYER;
		if (auto graphic = getGraphicComponent())
			layer = graphic->getLayerFromVariant(GraphicType::BASIC_GRAPHIC_INDEX).value_or(GraphicItemLayer::GUI_LAYER);

		buttonText = std::make_unique<MgeText>(butText, characterSize_pxls, FPoint(), false, layer);
		buttonText->setIsVisible(getIsVisible());
		buttonText->setColor(col);
		buttonText->setAbsolutePosition(getAlignedPosition(align, buttonText->getTextSize()));
	}
	else
	{
		_ASSERT(false); //not yet
	}
}

void MgeButton::setButtonTextColors(MgeColor defaultColor, MgeColor mouseOverColor)
{
	defaultTextColor = defaultColor;
	mouseOverTextColor = mouseOverColor;
	if (buttonText)
	{
		if (isUnderMouseCursor)
			buttonText->setColor(mouseOverTextColor);
		else
			buttonText->setColor(defaultTextColor);
	}
}

void MgeButton::onCursorEnterCall() noexcept
{
	try
	{
		setColor(mouseOverColor);

		if (buttonText)
			buttonText->setColor(mouseOverTextColor);
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
			
		if (buttonText)
			buttonText->setColor(defaultTextColor);
		MgeFrame::onCursorLeaveCall();
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

void MgeButton::setBasicCollision()
{
	editCollisions().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition().asInt(), getSize())));
	_ASSERT(editCollisions().size() == 1);
}

namespace mge
{
	Button mge::createButton(const FPoint& position, const ISize& size, GraphicItemLayer layer)
	{
		auto newButton = std::make_shared<MgeButton>(position, size, layer);
		newButton->initializeSelf(newButton);
		return newButton;
	}

	Button createButton(const FPoint& position, MgeImage&& image)
	{
		auto newButton = std::make_shared<MgeButton>(position, std::move(image));
		newButton->initializeSelf(newButton);
		return newButton;
	}
}

