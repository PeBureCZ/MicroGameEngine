#include "Button.h"

#include "MgeText.h"

Button::Button(const IPoint& newPosition, mgeType::Size<int> newSize)
	: Frame(newPosition, newSize)
	
{
	setColor(defaultColor);
	setBasicCollision();
}

Button::Button
	(
		const IPoint& newPosition,
		TextureId idUnselected,
		TextureId idSelected,
		TextureId idClicked
	)
	: Frame(newPosition, idUnselected)
	, unselectedTexture(idUnselected), selectedTexture(idSelected), clickedTexture(idClicked)
{
	setBasicCollision();
}

void Button::setDefaultButtonColor(const mgeType::Color_RGBA& newColor)
{
	defaultColor = newColor;
	if (!isUnderCursor())
		setColor(defaultColor);
}

void Button::setMouseOverButtonColor(const mgeType::Color_RGBA& newColor)
{
	mouseOverColor = newColor;
	if (isUnderCursor()) 
		setColor(mouseOverColor);
}

void Button::setIsVisible(bool visible) noexcept
{
	Frame::setIsVisible(visible);
}

void Button::setOnLMBClick(Callback_deprecated clickFunction) noexcept
{
	onLMBClick  = std::move(clickFunction);
}

void Button::setOnRMBClick(Callback_deprecated clickFunction) noexcept
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

void Button::setButtonTextColors(mgeType::Color_RGBA defaultColor, mgeType::Color_RGBA mouseOverColor)
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
	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition(), getSize())));
	_ASSERT(editCollision().size() == 1);
}
