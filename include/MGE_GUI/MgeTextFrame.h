#pragma once
#include "MgeFrame.h"

#include <deque>
#include <string>
#include <memory>

#include "GraphicDependencies.h"
#include "MgeText.h"

#include "BasicTypes.h"

class MgeText;
class MgeTextFrame;

constexpr MgeColor INVISIBLE_COLOR = MgeColor(0, 0, 0, 0);

namespace mge
{
	using TextFrame = std::shared_ptr<MgeTextFrame>;
	TextFrame createTextFrame(const FPoint& newPosition = FPoint(0.f, 0.f), mgeType::Size<int> newSize = mgeType::Size<int>(1, 1), std::string text = "", unsigned int characterSize_pxls = 30,
		GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER, MgeColor textColor = MgeColor(), MgeColor color = INVISIBLE_COLOR);
}

class MgeTextFrame : public MgeFrame
{
public:
	MgeTextFrame(const FPoint& newPosition = FPoint(0.f,0.f), mgeType::Size<int> newSize = mgeType::Size<int>(1,1), std::string text = "", unsigned int characterSize_pxls = 30,
		GraphicItemLayer layer = GraphicItemLayer::GUI_LAYER, MgeColor textColor = MgeColor(), MgeColor frameColor = INVISIBLE_COLOR);

	void addTextLine(std::string text, unsigned int textPxlsSize = 8, MgeColor color = MgeColor(0,0,0,255), bool bold = false) noexcept;

	void setPadding(int leftBorder_pxls, int topBorder_pxls, int betweenTextLine_pxls = 3);

	void removeLine(size_t index) noexcept;
	void removeFirstLine() noexcept;
	void removeLastLine() noexcept;

	void layout() noexcept override;

	void setTextsAlign(GuiAlign align = GuiAlign::TopLeft);
	void setTextColor(MgeColor newColor, size_t lineIndex = 0);

private:
	int actualTextLinePos_pxls = 0;
	int linePadding_pxls = 3;
	int leftPadding = 0;
	int topPadding = 0;
	int bottomPadding = 0;
	int rightPadding = 0;
	GuiAlign usedAlign = GuiAlign::TopLeft;
	std::deque<MgeText> frameTexts;

	[[nodiscard]] size_t getAllTextsHeight() noexcept;
	void redrawTextFrame() noexcept;
};

