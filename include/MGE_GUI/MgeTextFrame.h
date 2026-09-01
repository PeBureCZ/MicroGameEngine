#pragma once
#include "MgeFrame.h"

#include <vector>
#include <string>
#include <memory>

#include "GuiDependencies.h"

#include "BasicTypes.h"
#include "MgeText.h"

class MgeTextFrame : public MgeFrame
{
public:
	MgeTextFrame(const FPoint& newPosition = FPoint(0.f,0.f), mgeType::Size<int> newSize = mgeType::Size<int>(300,200));

	void addTextLine(std::string text, unsigned int textPxlsSize = 8, mgeType::Color_RGBA color = mgeType::Color_RGBA(0,0,0,255), bool bold = false) noexcept;

	void setPadding(int leftBorder_pxls, int topBorder_pxls, int betweenTextLine_pxls = 3);

	void removeLine(size_t index) noexcept;
	void removeFirstLine() noexcept;
	void removeLastLine() noexcept;

	void setTextsAlign(GuiAlign align = GuiAlign::TopLeft);

private:
	int actualTextLinePos_pxls = 0;
	int linePadding_pxls = 3;
	int leftPadding = 0;
	int topPadding = 0;
	GuiAlign usedAlign = GuiAlign::TopLeft;

	[[nodiscard]] size_t getAllTextsHeight() noexcept;
	void redrawTextFrame() noexcept;
};

