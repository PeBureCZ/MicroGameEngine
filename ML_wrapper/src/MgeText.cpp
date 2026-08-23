#include "MgeText.h"

#include "GlobalFunctions.h"

#include "MlWrapper.h"

static void normalizeSize(sf::Text& text)
{
	auto charSize = text.getCharacterSize();
	auto bounds = text.getLocalBounds();
	auto& glyph1 = text.getFont().getGlyph('x', charSize, false);
	auto& glyph2 = text.getFont().getGlyph('H', charSize, false);
	auto topShift = glyph2.textureRect.size.y - glyph1.textureRect.size.y;
	text.setOrigin(sf::Vector2f(text.getOrigin().x, text.getOrigin().y + topShift));
}

MgeText::MgeText(std::string newText, unsigned int characterSize_pxls, FPoint position, bool bold, size_t layer)
{
	const std::string fontPath = mgeCore::getExecutablePath() + "\\Fonts\\NotoSerifGeorgian-Regular.ttf";
	
	//THIS IS ONLY TEMP SOLUTION! FONT NEEDS TO BE SAVED SOMEWHERE TO USE IT!
	static sf::Font font; 
	static bool firstTimeOpen = true;
	if (firstTimeOpen)
	{
		if (!font.openFromFile(fontPath))
		{
			_ASSERT(false); //font loading failed
			return;
		}
		firstTimeOpen = false;
	}

	m_text = std::make_shared<sf::Text>(font, std::move(newText), characterSize_pxls);
	m_layer = layer;
	if (bold)
		m_text->setStyle(sf::Text::Bold);
	m_text->setPosition(sf::Vector2f(position.x, position.y));
	m_text->setFillColor(sf::Color::White);
	normalizeSize(*m_text); //move origin to font top without "ascender"
	ML_wrapper::getGlobalMlWrapper()->appendText(*this);
}

bool MgeText::operator==(const MgeText& other) const
{
	return this == &other;
}

bool MgeText::operator==(MgeText& other) const
{
	return this == &other;
}

void MgeText::setAbsolutePosition(IPoint newPosition)
{
	_ASSERT(m_text);
	if (m_text)
		m_text->setPosition(sf::Vector2f((float)newPosition.x, (float)newPosition.y));
}

void MgeText::setIsVisible(bool visible) noexcept
{
	m_isVisible = visible;
	if (m_text)
	{
		auto currentCollor = m_text->getFillColor();
		currentCollor.a = (visible) ? m_alpha : 0;
		m_text->setFillColor(currentCollor);
	}
}

void MgeText::setColor(mgeType::Color_RGBA newColor)
{
	_ASSERT(m_text);
	if (m_text)
	{
		m_alpha = newColor.a;
		m_text->setFillColor(sf::Color(newColor.r, newColor.g, newColor.b, m_isVisible ? newColor.a : 0));
	}
}

void MgeText::setBold(bool setBold)
{
	if (m_text)
		m_text->setStyle((setBold) ? sf::Text::Bold : sf::Text::Regular);
	else
		_ASSERT(false);
}

IPoint MgeText::getAbsolutePosition()
{
	if (m_text)
	{
		auto pos = m_text->getPosition();
		return IPoint((int)pos.x, (int)pos.y);
	}
	return IPoint();
}

mgeType::Size<int> MgeText::getTextSize() const
{
	if (m_text)
	{
		auto charSize = m_text->getCharacterSize();
		auto bounds = m_text->getLocalBounds();
		return mgeType::Size<int>(static_cast<int>(bounds.size.x), static_cast<int>(charSize));
	}
	else
	{ //text not created
		_ASSERT(false);
		return mgeType::Size<int>(0, 0);
	} 
}

std::shared_ptr<sf::Text> MgeText::getTextObject() const noexcept
{
	_ASSERT(m_text);
	return m_text;
}

[[nodiscard]] size_t MgeText::getLayer() const noexcept
{
	return m_layer;
}

MgeText::~MgeText()
{
	if (m_text)
	{
		ML_wrapper::getGlobalMlWrapper()->removeText(*this);
		_ASSERT(m_text.use_count() == 1); //correct = text live in MgeText only, not in any layer or another object
	}
	m_text.reset();
}
