#pragma once
#include <vector>
#include <memory>
#include <string>

#include "MgeFrame.h"

constexpr MgeColor DEFAULT_BUTTON_COLOR = MgeColor(160, 160, 160, 255);
constexpr MgeColor MOUSE_OVER_FRAME_COLOR = MgeColor(220, 220, 220, 255);
constexpr MgeColor DEFAULT_COLOR = MgeColor(140, 140, 140, 255);
constexpr MgeColor DISABLED_BUTTON_COLOR = MgeColor(200, 200, 200, 255);

constexpr unsigned int DEFAULT_BUT_TEXT_FONT_SIZE = 20;

/**state set by the owner; hover is not a state, it comes from the cursor (isUnderCursor()) and combines with Normal / Active*/
enum class ButtonState : int
{
	Normal,			//default look, hovered look under the cursor
	Active,			//selected: active look, hovered active look under the cursor
	Disabled,		//disabled look, ignores clicks and the cursor enter callback; it still blocks the cursor
	DisabledActive	//as Disabled; enabling it gives Active again
};

class MgeButton;

namespace mge
{
	using Button = std::shared_ptr<MgeButton>;
	Button createButton(const FPoint& position = FPoint(), const ISize& size = ISize(1, 1), GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
	Button createButton(const FPoint& position, MgeImage&& imgWithDefaultTexture, const TextureId& hovered,
		const TextureId& active, const TextureId& disabled, const TextureId& activeHovered, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
}

class MgeButton : public MgeFrame
{
public:
	MgeButton(const FPoint& newPosition, mgeType::Size<int> newSize, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);
	MgeButton(const FPoint& position, MgeImage&& imgWithDefaultTexture, const TextureId& hovered,
		const TextureId& active, const TextureId& disabled, const TextureId& activeHovered, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER);

	MgeButton(MgeButton&) = delete;
	MgeButton(MgeButton&&) = delete;

	MgeButton& operator= (MgeButton&) = default;
	MgeButton& operator= (MgeButton&&) = default;
	~MgeButton() = default;

	void setDisabledTexture(const TextureId& textureId);

	void setIsVisible(bool visible) noexcept override;

	void setState(ButtonState newState) noexcept;
	[[nodiscard]] ButtonState getState() const noexcept;

	/**keeps the active flag: Active <-> DisabledActive, Normal <-> Disabled*/
	void setIsDisabled(bool disabled) noexcept;
	/**true for Disabled and DisabledActive*/
	[[nodiscard]] bool getIsDisabled() const noexcept;

	/**keeps the disabled flag: Normal <-> Active, Disabled <-> DisabledActive*/
	void setIsActive(bool active) noexcept;
	/**true for Active and DisabledActive (a disabled button remembers it was active)*/
	[[nodiscard]] bool getIsActive() const noexcept;

	void setOnLMBClick(VoidCallback clickFunction) noexcept;
	void setOnRMBClick(VoidCallback clickFunction) noexcept;

	void onLmbClickCall() noexcept;
	void onRmbClickCall() noexcept;

	void layout() noexcept override;

	void setButtonTextColors(const MgeColor& defaultColor, const MgeColor& hoveredColor, const MgeColor& disableColor,
		const MgeColor& activeColor, const MgeColor& hoveredActiveColor);

	virtual void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) override;
	virtual void setColor(const MgeColor& color) noexcept override;
	void setHoveredColor(const MgeColor& color) noexcept;
	void setDisabledColor(const MgeColor& color) noexcept;
	void setActiveColor(const MgeColor& color) noexcept;
	void setHoveredActiveColor(const MgeColor& color) noexcept;
	void setButtonColors(const MgeColor& defaultColor, const MgeColor& hoveredColor, const MgeColor& disableColor,
		const MgeColor& activeColor, const MgeColor& hoveredActiveColor);

	void setButtonText(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align);
	void setButtonText(const std::string& butText);
	std::string getButtonText() const;
	MgeText& getButtonTextObject();

protected:
	virtual void onCursorEnterCall() noexcept override;
	virtual void onCursorLeaveCall() noexcept override;
	void applyDrawGroupToGraphics() noexcept override;

private:
	MgeColor m_defaultTextColor = DEFAULT_TEXT_COLOR;
	MgeColor m_hoveredTextColor = DEFAULT_TEXT_COLOR;
	MgeColor m_disabledTextColor = DISABLED_TEXT_COLOR;
	MgeColor m_activeTextColor = DEFAULT_TEXT_COLOR;
	MgeColor m_hoveredActiveTextColor = DEFAULT_TEXT_COLOR;

	MgeColor m_defaultColor = DEFAULT_BUTTON_COLOR;
	MgeColor m_hoveredColor = MOUSE_OVER_FRAME_COLOR;
	MgeColor m_disabledColor = DISABLED_BUTTON_COLOR;
	MgeColor m_activeColor = MOUSE_OVER_FRAME_COLOR;
	MgeColor m_hoveredActiveColor = MOUSE_OVER_FRAME_COLOR;

	TextureId m_defaultTexture = TextureId();
	TextureId m_hoveredTexture = TextureId();
	TextureId m_disabledTexture = TextureId();
	TextureId m_activeTexture = TextureId();
	TextureId m_hoveredActiveTexture = TextureId();

	ButtonState m_state = ButtonState::Normal;

	std::unique_ptr<MgeText> m_buttonText;

	void applyLook() noexcept;
	/**image buttons only; an undefined texture is ignored*/
	void setImageTexture(const TextureId& textureId);
	[[nodiscard]] const MgeColor& getTextColor() const noexcept;

	void addTextToButton(const std::string& butText, unsigned int characterSize_pxls, GuiAlign align, const MgeColor& col, int64_t zPos = mgeCore::getDefaultZPosition(mgeCore::ZPositionAdditionMode::Text));

	std::function<void()> onLMBClick = nullptr;
	std::function<void()> onRMBClick = nullptr;
};
