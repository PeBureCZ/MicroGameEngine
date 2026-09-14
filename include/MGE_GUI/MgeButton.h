#pragma once
#include <vector>
#include <memory>

#include "MgeFrame.h"
#include "GraphicDependencies.h"

constexpr mgeType::Color_RGBA DEFAULT_FRAME_COLOR = mgeType::Color_RGBA(160, 160, 160, 255);
constexpr mgeType::Color_RGBA MOUSE_OVER_FRAME_COLOR = mgeType::Color_RGBA(220, 220, 220, 255);
constexpr mgeType::Color_RGBA DEFAULT_COLOR = mgeType::Color_RGBA(140, 140, 140, 255);

class MgeButton;

namespace mge
{
	using Button = std::shared_ptr<MgeButton>;
	Button createButton(const FPoint& position = FPoint(), const ISize& size = ISize(1, 1));
	Button createButton(const FPoint& position, MgeImage&& image);
}

class MgeButton : public MgeFrame
{
public:
	MgeButton(const FPoint& newPosition, mgeType::Size<int> newSize);
	MgeButton
		(
			const FPoint& newPosition,
			const TextureId& idUnselected,
			const TextureId& idSelected = TextureId(),
			const TextureId& idClicked = TextureId()
		);

	MgeButton(const FPoint& newPosition, MgeImage&& image);

	MgeButton(MgeButton&) = delete;
	MgeButton(MgeButton&&) = delete;

	MgeButton& operator= (MgeButton&) = default;
	MgeButton& operator= (MgeButton&&) = default;
	~MgeButton() = default;

	void setDefaultButtonColor(const mgeType::Color_RGBA& newColor = DEFAULT_FRAME_COLOR);
	void setMouseOverButtonColor(const mgeType::Color_RGBA& newColor = MOUSE_OVER_FRAME_COLOR);
	void setIsVisible(bool visible) noexcept override;

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
	mgeType::Color_RGBA mouseOverTextColor = MOUSE_OVER_TEXT_COLOR;

	TextureId unselectedTexture = TextureId();
	TextureId selectedTexture = TextureId();
	TextureId clickedTexture = TextureId();

	void setBasicCollision();

	std::function<void()> onLMBClick = nullptr;
	std::function<void()> onRMBClick = nullptr;
};

