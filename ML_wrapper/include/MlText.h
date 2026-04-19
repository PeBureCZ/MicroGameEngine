#pragma once

#include <string>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include "BasicTypes.h"

#include <memory>

namespace sf
{
	class Font;
	class Text;
}

class MlText
{
public:
	explicit MlText
	(
		const sf::Font& font,
		std::string newText,
		unsigned int characterSize_pxls = 30,
		FPoint = FPoint(),
		bool bold = false
	);
	MlText() = delete;

	MlText(const MlText& other);
	MlText(MlText&& other) noexcept;

	MlText& operator=(MlText&& other) noexcept;
	MlText& operator=(const MlText&) = delete;

	void setAbsolutePosition(IPoint newPosition);
	[[nodiscard]] IPoint getAbsolutePosition();

	void setColor(tsmType::Color_RGBA newColor);
	void setBold(bool setBold);

	[[nodiscard]] tsmType::Size<int> getTextSize() const noexcept;
	[[nodiscard]] sf::Text* getTextObject() const noexcept;

	~MlText() = default;

private:
	std::unique_ptr< sf::Text> text;
};

