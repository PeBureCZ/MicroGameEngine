#pragma once
#include <vector>
#include <memory>

#include "Frame.h"
#include "GraphicDependencies.h"

constexpr mgeType::Color_RGBA DEFAULT_FRAME_COLOR = mgeType::Color_RGBA(230, 230, 230, 255);
constexpr mgeType::Color_RGBA DEFAULT_COLOR = mgeType::Color_RGBA(255, 255, 255, 255);
constexpr mgeType::Color_RGBA MOUSE_OVER_FRAME_COLOR = DEFAULT_COLOR;

constexpr mgeType::Color_RGBA DEFAULT_TEXT_COLOR = mgeType::Color_RGBA(61, 201, 245, 255);
constexpr mgeType::Color_RGBA MOUSE_OVER_TEXT_COLOR = mgeType::Color_RGBA(255, 153, 51, 255);

class Button : public Frame
{
public:
	Button(const IPoint& newPosition, mgeType::Size<int> newSize);
	Button
		(
			const IPoint& newPosition,
			TextureId idUnselected,
			TextureId idSelected = TextureId(),
			TextureId idClicked = TextureId()
		);
	~Button() = default;

	void setDefaultButtonColor(const mgeType::Color_RGBA& newColor = DEFAULT_FRAME_COLOR);
	void setMouseOverButtonColor(const mgeType::Color_RGBA& newColor = MOUSE_OVER_FRAME_COLOR);

	void setOnLMBClick(Callback_deprecated clickFunction) noexcept;
	void setOnRMBClick(Callback_deprecated clickFunction) noexcept;

	void onLmbClickCall() noexcept;
	void onRmbClickCall() noexcept;

	void setButtonTextColors(mgeType::Color_RGBA defaultColor, mgeType::Color_RGBA mouseOverColor);

protected:
	virtual void onCursorEnterCall() noexcept override;
	virtual void onCursorLeaveCall() noexcept override;

private:
	mgeType::Color_RGBA defaultColor = DEFAULT_FRAME_COLOR;
	mgeType::Color_RGBA mouseOverColor = MOUSE_OVER_FRAME_COLOR;
	mgeType::Color_RGBA defaultTextColor = DEFAULT_TEXT_COLOR;
	mgeType::Color_RGBA mouseOverTextColor = MOUSE_OVER_FRAME_COLOR;

	TextureId unselectedTexture = TextureId();
	TextureId selectedTexture = TextureId();
	TextureId clickedTexture = TextureId();

	void setBasicCollision();

	std::function<void()> onLMBClick = nullptr;
	std::function<void()> onRMBClick = nullptr;
};
