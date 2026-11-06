#pragma once
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "MgeWindow.h"

constexpr MgeColor DEFAULT_MESSAGE_BACKGROUND_COLOR = MgeColor(255, 255, 255);
constexpr MgeColor DEFAULT_MESSAGE_HEADER_COLOR = MgeColor(200, 200, 200);
constexpr MgeColor DEFAULT_MESSAGE_HEADER_TEXT_COLOR = MgeColor(240, 240, 240);
constexpr MgeColor DEFAULT_MESSAGE_LABEL_TEXT_COLOR = MgeColor(0, 0, 0);
constexpr MgeColor DEFAULT_MESSAGE_BUTTON_COLOR = MgeColor(245, 245, 245);
constexpr MgeColor DEFAULT_MESSAGE_MOUSE_OVER_BUTTON_COLOR = MgeColor(65, 107, 178);
constexpr MgeColor DEFAULT_MESSAGE_BUTTON_TEXT_COLOR = MgeColor(0, 0, 0);
constexpr MgeColor DEFAULT_MESSAGE_MOUSE_OVER_BUTTON_TEXT_COLOR = MgeColor(255, 255, 255);
constexpr MgeColor DEFAULT_MESSAGE_BORDER_COLOR = MgeColor(200, 200, 200);

class MgeButton;
class MgeMessageWindow;
class MgeTextFrame;

/**one line of the message text; a longer text is split into several labels (no automatic wrapping)*/
struct MgeMessageLabel
{
	std::wstring m_text;
};

/**the window closes on click, then m_onClick is called (nullptr = only close)*/
struct MgeMessageButton
{
	std::wstring m_text;
	std::function<void()> m_onClick;
};

namespace mge
{
	using MessageWindow = std::shared_ptr<MgeMessageWindow>;

	/**the window is initialized when it is added to its parent (addWidget()), not here*/
	template <typename... Parts>
	[[nodiscard]] MessageWindow createMessageWindow(std::wstring headerText, Parts&&... parts)
	{
		return std::make_shared<MgeMessageWindow>(std::move(headerText), std::forward<Parts>(parts)...);
	}
}

/**general-purpose message window centered in the render window: labels and buttons are shown in the order they are passed;
consecutive buttons share one row, every label is its own row; all labels share one align, all button rows another one.
A click on a button clears all button callbacks, closes the window, then calls the button's m_onClick. Not modal.
Every align / color setter can be called before or after the window is added to its parent; it applies to all labels / buttons.
@code
auto window = mge::createMessageWindow(L"Header",
	MgeMessageLabel{ L"Message line" },
	MgeMessageButton{ "Yes", [this]() { onYes(); } },
	MgeMessageButton{ "No", nullptr });
window->setLabelsAlign(GuiAlign::MiddleLeft); //optional, default MiddleCenter
screen->addWidget(window);
window->setDefaultButtonColor(MgeColor(230, 230, 230)); //also works after addWidget()
@endcode*/
class MgeMessageWindow : public MgeWindow
{
public:
	template <typename... Parts>
	explicit MgeMessageWindow(std::wstring headerText, Parts&&... parts)
		: MgeMessageWindow(makeParts(std::forward<Parts>(parts)...), std::move(headerText))
	{
	}

	MgeMessageWindow(const MgeMessageWindow&) = delete;
	MgeMessageWindow(MgeMessageWindow&&) = delete;
	MgeMessageWindow& operator=(const MgeMessageWindow&) = delete;
	MgeMessageWindow& operator=(MgeMessageWindow&&) = delete;
	~MgeMessageWindow() = default;

	/**only the horizontal part is used: MiddleLeft / MiddleCenter / MiddleRight (Top / Bottom variants assert, then work as Middle)*/
	void setLabelsAlign(GuiAlign newAlign) noexcept;
	/**only the horizontal part is used: MiddleLeft / MiddleCenter / MiddleRight (Top / Bottom variants assert, then work as Middle)*/
	void setButtonsAlign(GuiAlign newAlign) noexcept;
	[[nodiscard]] GuiAlign getLabelsAlign() const noexcept;
	[[nodiscard]] GuiAlign getButtonsAlign() const noexcept;

	void setBackgroundColor(const MgeColor& newColor);
	[[nodiscard]] MgeColor getBackgroundColor() const noexcept;
	void setHeaderColor(const MgeColor& newColor);
	[[nodiscard]] MgeColor getHeaderColor() const noexcept;
	void setHeaderTextColor(const MgeColor& newColor);
	[[nodiscard]] MgeColor getHeaderTextColor() const noexcept;
	/**text color of all labels*/
	void setDefaultLabelTextColor(const MgeColor& newColor);
	[[nodiscard]] MgeColor getDefaultLabelTextColor() const noexcept;
	/**color of all buttons when the mouse is not over them*/
	void setDefaultButtonColor(const MgeColor& newColor);
	[[nodiscard]] MgeColor getDefaultButtonColor() const noexcept;
	void setMouseOverButtonColor(const MgeColor& newColor);
	[[nodiscard]] MgeColor getMouseOverButtonColor() const noexcept;
	void setButtonTextColors(const MgeColor& newDefaultColor, const MgeColor& newMouseOverColor);
	[[nodiscard]] MgeColor getButtonTextColor() const noexcept;
	[[nodiscard]] MgeColor getMouseOverButtonTextColor() const noexcept;
	/**border of the window and of all buttons*/
	void setBorderColor(const MgeColor& newColor);
	[[nodiscard]] MgeColor getBorderColor() const noexcept;

	void initialize() noexcept override;

private:
	using MessagePart = std::variant<MgeMessageLabel, MgeMessageButton>;

	/**buttons m_buttons[m_firstButtonIndex, m_firstButtonIndex + m_buttonCount) share one row*/
	struct ButtonRow
	{
		size_t m_firstButtonIndex = 0;
		size_t m_buttonCount = 0;
		float m_y_pxls = 0.f;
	};

	std::wstring m_headerText;
	std::vector<MessagePart> m_parts; //emptied by initialize(), the content then lives in the widgets below
	std::weak_ptr<MgeFrame> m_headerFrame;
	std::weak_ptr<MgeTextFrame> m_headerTextFrame;
	std::vector<std::weak_ptr<MgeTextFrame>> m_labels;
	std::vector<std::weak_ptr<MgeButton>> m_buttons; //in the order they were passed
	std::vector<std::function<void()>> m_onClicks; //same index as m_buttons
	std::vector<ButtonRow> m_buttonRows;
	mge::Frame m_windowBackground;
	GuiAlign m_labelsAlign = GuiAlign::MiddleCenter;
	GuiAlign m_buttonsAlign = GuiAlign::MiddleCenter;
	MgeColor m_backgroundColor = DEFAULT_MESSAGE_BACKGROUND_COLOR;
	MgeColor m_headerColor = DEFAULT_MESSAGE_HEADER_COLOR;
	MgeColor m_headerTextColor = DEFAULT_MESSAGE_HEADER_TEXT_COLOR;
	MgeColor m_labelTextColor = DEFAULT_MESSAGE_LABEL_TEXT_COLOR;
	MgeColor m_buttonColor = DEFAULT_MESSAGE_BUTTON_COLOR;
	MgeColor m_mouseOverButtonColor = DEFAULT_MESSAGE_MOUSE_OVER_BUTTON_COLOR;
	MgeColor m_buttonTextColor = DEFAULT_MESSAGE_BUTTON_TEXT_COLOR;
	MgeColor m_mouseOverButtonTextColor = DEFAULT_MESSAGE_MOUSE_OVER_BUTTON_TEXT_COLOR;
	MgeColor m_borderColor = DEFAULT_MESSAGE_BORDER_COLOR;

	MgeMessageWindow(std::vector<MessagePart>&& parts, std::wstring headerText);

	template <typename... Parts>
	[[nodiscard]] static std::vector<MessagePart> makeParts(Parts&&... parts)
	{
		static_assert(((std::is_same_v<std::remove_cvref_t<Parts>, MgeMessageLabel>
			|| std::is_same_v<std::remove_cvref_t<Parts>, MgeMessageButton>) && ...),
			"MgeMessageWindow accepts only MgeMessageLabel and MgeMessageButton parts");

		std::vector<MessagePart> messageParts;
		messageParts.reserve(sizeof...(Parts));
		(messageParts.emplace_back(std::forward<Parts>(parts)), ...);
		return messageParts;
	}

	[[nodiscard]] static ISize getWindowSize(const std::vector<MessagePart>& parts) noexcept;
	/**a label is one row, consecutive buttons are one row; returns the index after the row that starts at firstIndex*/
	[[nodiscard]] static size_t getRowEnd(const std::vector<MessagePart>& parts, size_t firstIndex) noexcept;
	[[nodiscard]] static bool isButton(const MessagePart& part) noexcept;
	[[nodiscard]] static int getButtonRowWidth(size_t buttonCount) noexcept;
	/**x of the first button of a row with buttonCount buttons, by m_buttonsAlign*/
	[[nodiscard]] float getButtonRowX(size_t buttonCount) const noexcept;
	/**true after initialize() created the labels and buttons*/
	[[nodiscard]] bool hasContent() const noexcept;

	/**creates the widgets; aligns and colors are set later by applyStyle()*/
	void createBackgroundWithHeader();
	void createLabel(const MgeMessageLabel& label, float y);
	/**creates the buttons parts(firstIndex, endIndex) in one row*/
	void createButtonRow(size_t firstIndex, size_t endIndex, float y);

	/**the apply...() functions push the stored values to the created widgets (nothing before initialize()); layout() is up to the caller*/
	void applyStyle() noexcept; //all apply...() below
	void applyLabelsAlign() noexcept;
	void applyButtonsAlign() noexcept;
	void applyBackgroundColor() noexcept;
	void applyHeaderColor() noexcept;
	void applyHeaderTextColor() noexcept;
	void applyLabelsTextColor() noexcept;
	void applyButtonsColors() noexcept; //default, mouse over and text colors
	void applyBorderColor() noexcept; //window and buttons
	/**calls action for every existing label / button; an exception asserts and stops the loop*/
	void forEachLabel(const std::function<void(MgeTextFrame&)>& action) noexcept;
	void forEachButton(const std::function<void(MgeButton&)>& action) noexcept;

	void onButtonClick(size_t buttonIndex);
	/**clears the button callbacks (they reference this window) and closes it*/
	void closeMessage();
};
