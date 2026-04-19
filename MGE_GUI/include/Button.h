#pragma once
#include <vector>
#include <memory>

#include "Frame.h"
#include "GraphicDependencies.h"

constexpr tsmType::Color_RGBA DEFAULT_FRAME_COLOR = tsmType::Color_RGBA(230, 230, 230, 255);
constexpr tsmType::Color_RGBA MOUSE_OVER_FRAME_COLOR = tsmType::Color_RGBA(255, 255, 255, 255);

constexpr tsmType::Color_RGBA DEFAULT_TEXT_COLOR = tsmType::Color_RGBA(61, 201, 245, 255);
constexpr tsmType::Color_RGBA MOUSE_OVER_TEXT_COLOR = tsmType::Color_RGBA(255, 153, 51, 255);

class Button : public Frame
{
public:
	Button(IPoint newPosition, tsmType::Size<int> newSize);
	Button
		(
			IPoint newPosition,
			TextureId idUnselected,
			TextureId idSelected = TextureId(),
			TextureId idClicked = TextureId()
		);
	~Button() = default;

	void setDefaultButtonColor(tsmType::Color_RGBA newColor = DEFAULT_FRAME_COLOR);
	void setMouseOverButtonColor(tsmType::Color_RGBA newColor = MOUSE_OVER_FRAME_COLOR);

	void setOnLMBClick(Callback clickFunction);
	void setOnRMBClick(Callback clickFunction);

	void onLmbClickCall();
	void onRmbClickCall();

	void setButtonTextColors(tsmType::Color_RGBA defaultColor, tsmType::Color_RGBA mouseOverColor);

protected:
	virtual void onCursorEnterCall() noexcept override;
	virtual void onCursorLeaveCall() noexcept override;

private:
	tsmType::Color_RGBA defaultColor = DEFAULT_FRAME_COLOR;
	tsmType::Color_RGBA mouseOverColor = MOUSE_OVER_FRAME_COLOR;
	tsmType::Color_RGBA defaultTextColor = DEFAULT_TEXT_COLOR;
	tsmType::Color_RGBA mouseOverTextColor = MOUSE_OVER_FRAME_COLOR;

	TextureId unselectedTexture = TextureId();
	TextureId selectedTexture = TextureId();
	TextureId clickedTexture = TextureId();

	void setBasicCollision();

	std::function<void()> onLMBClick = nullptr;
	std::function<void()> onRMBClick = nullptr;
};
