#include "MgeButton.h"

#include "MgeText.h"

MgeButton::MgeButton(const IPoint& newPosition, mgeType::Size<int> newSize)
	: MgeFrame(newPosition, newSize)
	
{
	setColor(defaultColor);
	setBasicCollision();
}

MgeButton::MgeButton
	(
		const IPoint& newPosition,
		TextureId idUnselected,
		TextureId idSelected,
		TextureId idClicked
	)
	: MgeFrame(newPosition, idUnselected)
	, unselectedTexture(idUnselected), selectedTexture(idSelected), clickedTexture(idClicked)
{
	setBasicCollision();
}

void MgeButton::setDefaultButtonColor(const mgeType::Color_RGBA& newColor)
{
	defaultColor = newColor;
	if (!isUnderCursor())
		setColor(defaultColor);
}

void MgeButton::setMouseOverButtonColor(const mgeType::Color_RGBA& newColor)
{
	mouseOverColor = newColor;
	if (isUnderCursor()) 
		setColor(mouseOverColor);
}

void MgeButton::setIsVisible(bool visible) noexcept
{
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

void MgeButton::setButtonTextColors(mgeType::Color_RGBA defaultColor, mgeType::Color_RGBA mouseOverColor)
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

void MgeButton::onCursorEnterCall() noexcept
{
	try
	{
		setColor(mouseOverColor);

		if (frameTexts.size() > 0)
		{
			for (auto& text : frameTexts)
				text.second.setColor(mouseOverTextColor);
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
			
		if (frameTexts.size() > 0)
		{
			for (auto& text : frameTexts)
				text.second.setColor(defaultTextColor);
		}
		MgeFrame::onCursorLeaveCall();
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

void MgeButton::setBasicCollision()
{
	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition(), getSize())));
	_ASSERT(editCollision().size() == 1);
}
