#pragma once

#include <string>
#include <memory>
#include <variant>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include "SFML/Graphics.hpp"
#include "BasicTypes.h"
#include "GraphicDependencies.h"

namespace sf
{
	class Font;
	class Text;
}

struct MgeLayerObject;

class MgeText
{
public:
	explicit MgeText
		(
			std::string newText,
			unsigned int characterSize_pxls = 30,
			const FPoint& position = FPoint(),
			bool bold = false,
			size_t layer = GraphicItemLayer::GUI_LAYER,
			const MgeColor& color = MgeColor()
		);
	MgeText() = delete;

	MgeText(const MgeText& other) = delete;
	MgeText(MgeText&& other) noexcept = default;
	MgeText& operator=(MgeText&& other) noexcept;
	MgeText& operator=(const MgeText&) = delete;
	bool operator==(const MgeText& other) const;
	bool operator==(MgeText& other) const;

	void setAbsolutePosition(IPoint newPosition);
	[[nodiscard]] IPoint getAbsolutePosition();

	void setIsVisible(bool visible) noexcept;
	void setColor(const MgeColor& newColor);
	void setText(const std::string& newText);
	void setText(const std::wstring& newText);
	void appendText(const std::string& newText);
	void appendText(const std::wstring& newText);
	void removeLastCharacter();

	void setFontSize(unsigned int newSize);
	unsigned int getFontSize() const;
	[[nodiscard]] std::string getText() const;
	[[nodiscard]] MgeColor getColor() const noexcept;
	[[nodiscard]] bool isVisible() const noexcept;
	[[nodiscard]] bool isBold() const noexcept;
	[[nodiscard]] bool isValid() const noexcept;
	void setBold(bool setBold);
	[[nodiscard]] GuiAlign getAlign() const noexcept;
	void setAlign(GuiAlign align) noexcept;

	[[nodiscard]] mgeType::Size<int> getTextSize() const noexcept;
	[[nodiscard]] std::shared_ptr<MgeLayerObject> getTextObject() const noexcept;
	[[nodiscard]] size_t getLayer() const noexcept;
	void setLayer(size_t newLayer);
	void setZPosition(int64_t newZPosition) noexcept;
	[[nodiscard]] int64_t getZPosition() const noexcept;

	~MgeText();

private:
	void destroyText() noexcept;

	std::weak_ptr<MgeLayerObject> m_text;
	std::uint8_t m_alpha = 255;
	bool m_isVisible = true;
	GuiAlign m_align = GuiAlign::MiddleCenter;
};

