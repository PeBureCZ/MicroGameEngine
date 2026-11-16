#pragma once
#include "MgeFrame.h"

#include <deque>
#include <string>
#include <memory>
#include <vector>

#include "GraphicDependencies.h"
#include "MgeText.h"
#include "MgeTextInputRules.h"

#include "BasicTypes.h"

class MgeText;
class MgeTextFrame;

constexpr MgeColor INVISIBLE_COLOR = MgeColor(0, 0, 0, 0);
constexpr MgeColor DEFAULT_TEXT_FRAME_COLOR = MgeColor(160, 160, 160);
constexpr MgeColor DEFAULT_TEXT_FRAME_COLOR_UNDER_EDIT = MgeColor(168, 168, 175);
constexpr unsigned int DEFAULT_TEXT_SIZE_PXLS = 12;

namespace mge
{
	using TextFrame = std::shared_ptr<MgeTextFrame>;
	TextFrame createTextFrame(const FPoint& newPosition = FPoint(0.f, 0.f), const ISize& size = ISize(1, 1), std::string text = "",
		unsigned int characterSize_pxls = DEFAULT_TEXT_SIZE_PXLS, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER,
		const MgeColor& textColor = MgeColor(), const MgeColor& color = INVISIBLE_COLOR);

	TextFrame createTextFrame(const FPoint& newPosition = FPoint(0.f, 0.f), const ISize& size = ISize(1, 1), const std::wstring& text = L"",
		unsigned int characterSize_pxls = DEFAULT_TEXT_SIZE_PXLS, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER,
		const MgeColor& textColor = MgeColor(), const MgeColor& color = INVISIBLE_COLOR);

	TextFrame createEditableTextFrame(const FPoint& position = FPoint(0.f, 0.f), const ISize& size = ISize(250, 20), const std::wstring& text = L"",
		unsigned int characterSize_pxls = DEFAULT_TEXT_SIZE_PXLS, GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER,
		const MgeColor& textColor = MgeColor(), const MgeColor& color = DEFAULT_TEXT_FRAME_COLOR);
}

class MgeTextFrame : public MgeFrame
{
public:
	MgeTextFrame(const FPoint& newPosition = FPoint(0.f,0.f), const ISize& size = ISize(1,1), std::string text = "", unsigned int characterSize_pxls = 30,
		GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER, MgeColor textColor = MgeColor(), MgeColor frameColor = INVISIBLE_COLOR, bool editable = false);

	[[nodiscard]] unsigned int getDefaultNewTextSize() const noexcept;
	void setDefaultNewTextSize(unsigned int height_pxls) noexcept;
	void setDefaultTextColor(const MgeColor& color) noexcept;
	void addTextLine(std::string text_utf8, unsigned int height_pxls = DEFAULT_TEXT_SIZE_PXLS, MgeColor color = MgeColor(0, 0, 0, 255), bool bold = false) noexcept;
	void addTextLine(std::wstring text, unsigned int height_pxls = DEFAULT_TEXT_SIZE_PXLS, MgeColor color = MgeColor(0, 0, 0, 255), bool bold = false) noexcept;
	void appendTextToLine(const std::string& text_utf8, size_t lineIndex);
	const std::deque<MgeText>& getTextLines() const;
	std::deque<MgeText>& editTextLines();

	void setIsVisible(bool visible) noexcept override;

	void setPadding(int leftPadding_pxls = 0, int topPadding_pxls = 0, int rightPadding_pxls = 0,
		int botPadding_pxls = 0, int betweenTextLine_pxls = 3);

	/**@brief Enables or disables text editing and configures automatic frame coloring.
	@param editable If true, enables focus and registers the editing collision area.
	@param defaultFrameColoring If true, automatically changes the frame color when editing starts or ends.
	@note Calling setColor() can override the automatic under-edit coloring until the next focus change.
	@see setUnderEditColors()*/
	void setEditable(bool editable, bool defaultFrameColoring = true);

	/**@brief Sets the frame colors used when editing starts and ends.
	@param nonEditColor The frame color used when the frame is not being edited
	@param underEditColor The frame color used while the frame is being edited
	@note These colors are applied automatically only if defaultFrameColoring is enabled.
	@note Calling setColor() can override the under-edit color until the next focus change.
	@see setEditable()*/
	void setUnderEditColors(const MgeColor& nonEditColor, const MgeColor& underEditColor);

	/**@brief Adds a rule that filters characters typed by the user into the last (edited) line.
	@note A typed character is accepted only if every added rule accepts it. Without rules everything is accepted.
	@note Only typed characters are filtered. addTextLine(), appendTextToLine(), Enter (new line) and Backspace are not validated.
	@note While at least one rule is set, characters above U+FFFF (surrogate pairs) are rejected.
	@see TextInputRule, mge::inputRules*/
	void addInputRule(TextInputRule newRule) noexcept;
	void clearInputRules() noexcept;

	void removeLine(size_t index) noexcept;
	void removeFirstLine() noexcept;
	void removeLastLine() noexcept;
	void clear() noexcept;

	void layout() noexcept override;

	void setTextsAlign(GuiAlign align = GuiAlign::TopLeft);
	void setTextLineColor(const MgeColor& newColor, size_t lineIndex = 0);

	void onFocusGet() noexcept override;
	void onFocusLost() noexcept override;

protected:
	void applyDrawGroupToGraphics() noexcept override;

private:
	int linePadding_pxls = 3;
	int m_leftPadding = 0;
	int m_topPadding = 0;
	int m_bottomPadding = 0;
	int m_rightPadding = 0;
	bool m_isUnderEdit = false;
	bool m_defaultFrameColoring = true;
	MgeColor m_underEditColor = DEFAULT_TEXT_FRAME_COLOR_UNDER_EDIT;
	MgeColor m_nonEditColor = DEFAULT_TEXT_FRAME_COLOR;
	MgeColor m_defaultTextColor = DEFAULT_TEXT_COLOR;
	GuiAlign usedAlign = GuiAlign::TopLeft;
	std::deque<MgeText> m_frameTexts;
	std::vector<TextInputRule> m_inputRules;

	unsigned int m_defaultNewTextSize_pxls = DEFAULT_TEXT_SIZE_PXLS;

	[[nodiscard]] size_t getAllTextsHeight() noexcept;
	void redrawTextFrame() noexcept;
	void processKeyPressEvent(const KeyPressEvent& eventData);
	void processTextKeyEnteredEvent(const TextKeyEnteredEvent& eventData);
	[[nodiscard]] bool isTypedTextAllowed(const std::string& typedText_utf8) const noexcept;
};
