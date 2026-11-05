#pragma once
#include <vector>
#include <memory>
#include <string>

#include "MgeFrame.h"

constexpr MgeColor DEFAULT_BUTTON_COLOR = MgeColor(160, 160, 160, 255);
constexpr MgeColor MOUSE_OVER_FRAME_COLOR = MgeColor(220, 220, 220, 255);
constexpr MgeColor DEFAULT_COLOR = MgeColor(140, 140, 140, 255);

constexpr unsigned int DEFAULT_BUT_TEXT_FONT_SIZE = 20;

class MgeButton;

namespace mge
{
	using Button = std::shared_ptr<MgeButton>;
	Button createButton(const FPoint& position = FPoint(), const ISize& size = ISize(1, 1), GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
	Button createButton(const FPoint& position, MgeImage&& image, const TextureId& idHovered = TextureId(),
		const TextureId& idClicked = TextureId(), GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
}

class MgeButton : public MgeFrame
{
public:
	MgeButton(const FPoint& newPosition, mgeType::Size<int> newSize, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
	MgeButton(const FPoint& newPosition, MgeImage&& image, const TextureId& idHovered = TextureId(),
		const TextureId& idClicked = TextureId(), GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);

	MgeButton(MgeButton&) = delete;
	MgeButton(MgeButton&&) = delete;

	MgeButton& operator= (MgeButton&) = default;
	MgeButton& operator= (MgeButton&&) = default;
	~MgeButton() = default;

	void setDefaultButtonColor(const MgeColor& newColor = DEFAULT_BUTTON_COLOR);
	void setMouseOverButtonColor(const MgeColor& newColor = MOUSE_OVER_FRAME_COLOR);
	void setIsVisible(bool visible) noexcept override;

	void setOnLMBClick(VoidCallback clickFunction) noexcept;
	void setOnRMBClick(VoidCallback clickFunction) noexcept;

	void onLmbClickCall() noexcept;
	void onRmbClickCall() noexcept;

	void layout() noexcept override;

	void setButtonTextColors(MgeColor defaultColor, MgeColor mouseOverColor);
	void setButtonText(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align);
	void setButtonText(const std::string& butText);
	std::string getButtonText() const;
	MgeText& getButtonTextObject();

protected:
	virtual void onCursorEnterCall() noexcept override;
	virtual void onCursorLeaveCall() noexcept override;

private:
	MgeColor defaultColor = DEFAULT_BUTTON_COLOR;
	MgeColor mouseOverColor = MOUSE_OVER_FRAME_COLOR;
	MgeColor m_defaultTextColor = DEFAULT_TEXT_COLOR;
	MgeColor m_mouseOverTextColor = MOUSE_OVER_TEXT_COLOR;

	TextureId unselectedTexture = TextureId();
	TextureId m_hoveredTexture = TextureId();
	TextureId clickedTexture = TextureId();

	std::unique_ptr<MgeText> m_buttonText;

	void addTextToButton(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align, const MgeColor& col, int64_t zPos = mgeCore::getDefaultZPosition(mgeCore::ZPositionAdditionMode::Text));

	std::function<void()> onLMBClick = nullptr;
	std::function<void()> onRMBClick = nullptr;
};

