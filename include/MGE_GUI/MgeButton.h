#pragma once
#include <vector>
#include <memory>

#include "MgeFrame.h"

constexpr MgeColor DEFAULT_BUTTON_COLOR = MgeColor(160, 160, 160, 255);
constexpr MgeColor MOUSE_OVER_FRAME_COLOR = MgeColor(220, 220, 220, 255);
constexpr MgeColor DEFAULT_COLOR = MgeColor(140, 140, 140, 255);

class MgeButton;

namespace mge
{
	using Button = std::shared_ptr<MgeButton>;
	Button createButton(const FPoint& position = FPoint(), const ISize& size = ISize(1, 1), GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
	Button createButton(const FPoint& position, MgeImage&& image);
}

class MgeButton : public MgeFrame
{
public:
	MgeButton(const FPoint& newPosition, mgeType::Size<int> newSize, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
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

	void setDefaultButtonColor(const MgeColor& newColor = DEFAULT_BUTTON_COLOR);
	void setMouseOverButtonColor(const MgeColor& newColor = MOUSE_OVER_FRAME_COLOR);
	void setIsVisible(bool visible) noexcept override;

	void setOnLMBClick(Callback_deprecated clickFunction) noexcept;
	void setOnRMBClick(Callback_deprecated clickFunction) noexcept;

	void onLmbClickCall() noexcept;
	void onRmbClickCall() noexcept;

	void layout() noexcept override;

	void addTextToButton(const std::string& butText, unsigned int characterSize_pxls = 30, GuiAlign align = GuiAlign::MiddleCenter,
		const MgeColor& col = DEFAULT_TEXT_COLOR);

	void setButtonTextColors(MgeColor defaultColor, MgeColor mouseOverColor);

protected:
	virtual void onCursorEnterCall() noexcept override;
	virtual void onCursorLeaveCall() noexcept override;

private:
	MgeColor defaultColor = DEFAULT_BUTTON_COLOR;
	MgeColor mouseOverColor = MOUSE_OVER_FRAME_COLOR;
	MgeColor defaultTextColor = DEFAULT_TEXT_COLOR;
	MgeColor mouseOverTextColor = MOUSE_OVER_TEXT_COLOR;

	TextureId unselectedTexture = TextureId();
	TextureId selectedTexture = TextureId();
	TextureId clickedTexture = TextureId();

	std::unique_ptr<MgeText> buttonText;

	void setBasicCollision();

	std::function<void()> onLMBClick = nullptr;
	std::function<void()> onRMBClick = nullptr;
};

