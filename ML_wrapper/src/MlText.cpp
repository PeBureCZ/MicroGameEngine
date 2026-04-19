#include "MlText.h"

#include "SFML\Graphics.hpp"

static void normalizeSize(sf::Text& text)
{
	auto charSize = text.getCharacterSize();
	auto bounds = text.getLocalBounds();
	auto& glyph1 = text.getFont().getGlyph('x', charSize, false);
	auto& glyph2 = text.getFont().getGlyph('H', charSize, false);
	auto topShift = glyph2.textureRect.size.y - glyph1.textureRect.size.y;
	text.setOrigin(sf::Vector2f(text.getOrigin().x, text.getOrigin().y + topShift));
}

MlText::MlText(const sf::Font& font, std::string newText, unsigned int characterSize_pxls, FPoint position, bool bold)
{
	text = std::make_unique<sf::Text>(font, std::move(newText), characterSize_pxls);
	if (bold)
		text->setStyle(sf::Text::Bold);
	text->setPosition(sf::Vector2f(position.x, position.y));
	text->setFillColor(sf::Color::White);

	normalizeSize(*text); //move origin to font top without "ascender"
}

MlText::MlText(const MlText& other)
{
	text = std::make_unique<sf::Text>(*other.text);
}

MlText::MlText(MlText&& other) noexcept
{
	text = std::move(other.text);
	other.text.reset();
}

MlText& MlText::operator=(MlText&& other) noexcept
{
	if (this != &other)
	{
		text = std::move(other.text);
	}
	return *this;
}

void MlText::setAbsolutePosition(IPoint newPosition)
{
	if (text)
		text->setPosition(sf::Vector2f((float)newPosition.x, (float)newPosition.y));
	else
		{_ASSERT(false); }//text not created
}

void MlText::setColor(tsmType::Color_RGBA newColor)
{
	if (text)
		text->setFillColor(sf::Color(newColor.r, newColor.g, newColor.b, newColor.a));
	else
	{
		_ASSERT(false);
	} //text not created
}

void MlText::setBold(bool setBold)
{
	text->setStyle((setBold) ? sf::Text::Bold : sf::Text::Regular);
}


IPoint MlText::getAbsolutePosition()
{
	if (text)
	{
		auto pos = text->getPosition();
		return IPoint((int)pos.x, (int)pos.y);
	}
	return IPoint();
}

tsmType::Size<int> MlText::getTextSize() const noexcept
{
	if (text)
	{
		auto charSize = text->getCharacterSize();
		auto bounds = text->getLocalBounds();
		return tsmType::Size<int>(static_cast<int>(bounds.size.x), static_cast<int>(charSize));
	}
	else
	{ //text not created
		_ASSERT(false);
		return tsmType::Size<int>(0, 0);
	} 
}

sf::Text* MlText::getTextObject() const noexcept
{
	_ASSERT(text);
	return text.get();
}
