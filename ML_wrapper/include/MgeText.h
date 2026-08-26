#pragma once

#include <string>
#include <memory>

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
			FPoint = FPoint(),
			bool bold = false,
			size_t layer = GraphicItemLayer::GUI_LAYER
		);
	MgeText() = delete;

	MgeText(const MgeText& other) = delete;
	MgeText(MgeText&& other) = default;
	MgeText& operator=(MgeText&& other) = default;
	MgeText& operator=(const MgeText&) = delete;
	bool operator==(const MgeText& other) const;
	bool operator==(MgeText& other) const;

	void setAbsolutePosition(IPoint newPosition);
	[[nodiscard]] IPoint getAbsolutePosition();

	void setIsVisible(bool visible) noexcept;
	void setColor(mgeType::Color_RGBA newColor);
	void setBold(bool setBold);

	[[nodiscard]] mgeType::Size<int> getTextSize() const;
	[[nodiscard]] std::shared_ptr<MgeLayerObject> getTextObject() const noexcept;
	[[nodiscard]] size_t getLayer() const noexcept;
	void setZPosition(int64_t newZPosition) noexcept;
	[[nodiscard]] int64_t getZPosition() const noexcept;

	[[nodiscard]] 

	~MgeText();

private:
	std::weak_ptr<MgeLayerObject> m_text;
	std::uint8_t m_alpha = 255;
	bool m_isVisible = true;
};

