#include "Button.h"

#include "MlText.h"

Button::Button(IPoint newPosition, tsmType::Size<int> newSize)
	: Frame(newPosition, newSize)
	
{
	setColor(defaultColor);
	setBasicCollision();
}

Button::Button
	(
		IPoint newPosition,
		TextureId idUnselected,
		TextureId idSelected,
		TextureId idClicked
	)
	: Frame(newPosition, idUnselected)
	, unselectedTexture(idUnselected), selectedTexture(idSelected), clickedTexture(idClicked)
{
	setBasicCollision();
}



void Button::setDefaultButtonColor(tsmType::Color_RGBA newColor)
{
	defaultColor = newColor;
}

void Button::setMouseOverButtonColor(tsmType::Color_RGBA newColor)
{
	mouseOverColor = newColor;
}

void Button::setOnLMBClick(Callback clickFunction) noexcept
{
	onLMBClick  = std::move(clickFunction);
}

void Button::setOnRMBClick(Callback clickFunction) noexcept
{
	onRMBClick = std::move(clickFunction);
}

void Button::onLmbClickCall() noexcept
{
	if (onLMBClick)
		onLMBClick();
}

void Button::onRmbClickCall() noexcept
{
	if (onRMBClick)
		onRMBClick();
}

void Button::setButtonTextColors(tsmType::Color_RGBA defaultColor, tsmType::Color_RGBA mouseOverColor)
{
	defaultTextColor = defaultColor;
	mouseOverTextColor = mouseOverColor;
	if (frameTexts.size() > 0)
	{
		if (isUnderMouseCursor)
		{
			for (auto& text : frameTexts)
				text.second.setColor(mouseOverTextColor);
		}
		else
		{
			for (auto& text : frameTexts)
				text.second.setColor(defaultTextColor);
		}
	}
}

void Button::onCursorEnterCall() noexcept
{
	try
	{
		if (std::holds_alternative<DrawableObject<float>>(getFrameObject()))
			setColor(mouseOverColor);
		if (std::holds_alternative<std::shared_ptr<MlImage>>(getFrameObject())
			&& selectedTexture.path != UNDEFINED_TEXTURE_PATH)
		{
			setImage(selectedTexture);
		}
		else if (std::holds_alternative<std::shared_ptr<MlImage>>(getFrameObject()))
			setColor(mouseOverColor);
		if (frameTexts.size() > 0)
		{

			for (auto& text : frameTexts)
				text.second.setColor(mouseOverTextColor);
		}
		Frame::onCursorEnterCall();
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

void Button::onCursorLeaveCall() noexcept
{
	try
	{
		if (std::holds_alternative<DrawableObject<float>>(getFrameObject()))
			setColor(defaultColor);

		if (std::holds_alternative<std::shared_ptr<MlImage>>(getFrameObject())
			&& selectedTexture.path != UNDEFINED_TEXTURE_PATH
			&& unselectedTexture.path != UNDEFINED_TEXTURE_PATH)
		{
			setImage(unselectedTexture);
		}
		else if (std::holds_alternative<std::shared_ptr<MlImage>>(getFrameObject()))
			setColor(defaultColor);
		if (frameTexts.size() > 0)
		{
			for (auto& text : frameTexts)
				text.second.setColor(defaultTextColor);
		}
		Frame::onCursorLeaveCall();
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

void Button::setBasicCollision()
{
	editCollision().push_back(Trigger<int>(true, tsmShape::Rectangle<int>(getAbsolutePosition(), getSize())));
	_ASSERT(editCollision().size() == 1);
}
