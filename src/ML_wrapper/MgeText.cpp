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


	auto newMgeText = std::make_shared<MgeLayerObject>(MgeLayerObject{ mgeCore::getDefaultZPosition(), layer, sf::Text{font, std::move(newText), characterSize_pxls} });
	m_text = newMgeText;
	auto& sfText = std::get<sf::Text>(newMgeText->data);

	if (bold)
		sfText.setStyle(sf::Text::Bold);

	sfText.setPosition(sf::Vector2f(position.x, position.y));
	sfText.setFillColor(sf::Color::White);
	normalizeSize(sfText); //move origin to font top without "ascender"
	ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(std::move(newMgeText));
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
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			auto& sfText = std::get<sf::Text>(text->data);
			sfText.setPosition(sf::Vector2f((float)newPosition.x, (float)newPosition.y));
		}
		else
		{
			_ASSERT(false); //wrong type
		}
	}
}

void MgeText::setIsVisible(bool visible) noexcept
{
	m_isVisible = visible;
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			auto& sfText = std::get<sf::Text>(text->data);
			auto currentCollor = sfText.getFillColor();
			currentCollor.a = (visible) ? m_alpha : 0;
			sfText.setFillColor(currentCollor);
		}
		else
		{
			_ASSERT(false); //wrong type
		}
	}
}

void MgeText::setColor(mgeType::Color_RGBA newColor)
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			auto& sfText = std::get<sf::Text>(text->data);
			m_alpha = newColor.a;
			sfText.setFillColor(sf::Color(newColor.r, newColor.g, newColor.b, m_isVisible ? newColor.a : 0));
		}
		else
		{
			_ASSERT(false); //wrong type
		}
	}
}

void MgeText::setBold(bool setBold)
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			auto& sfText = std::get<sf::Text>(text->data);
			sfText.setStyle((setBold) ? sf::Text::Bold : sf::Text::Regular);
		}
		else
		{
			_ASSERT(false); //wrong type
		}
	}
}

IPoint MgeText::getAbsolutePosition()
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			auto& sfText = std::get<sf::Text>(text->data);
			auto pos = sfText.getPosition();
			return IPoint((int)pos.x, (int)pos.y);
		}
	}
	_ASSERT(false); //wrong type
	return IPoint();
}

mgeType::Size<int> MgeText::getTextSize() const
{
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			auto& sfText = std::get<sf::Text>(text->data);
			auto charSize = sfText.getCharacterSize();
			auto bounds = sfText.getLocalBounds();
			return mgeType::Size<int>(static_cast<int>(bounds.size.x), static_cast<int>(charSize));
		}
	}
	_ASSERT(false);
	return mgeType::Size<int>(0, 0);
}

std::shared_ptr<MgeLayerObject> MgeText::getTextObject() const noexcept
{
	return m_text.lock();
}

[[nodiscard]] size_t MgeText::getLayer() const noexcept
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
		return text->m_layer;
	else
		return 0;
}

void MgeText::setZPosition(const int64_t newZPosition) noexcept
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		text->zPosition = newZPosition;
		ML_wrapper::getGlobalMlWrapper()->sortLayerByZIndex_delayed(getLayer());
	}
}

int64_t MgeText::getZPosition() const noexcept
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
		return text->zPosition;
	else
		return 0;
;}

MgeText::~MgeText()
{
	if (auto text = m_text.lock())
	{
		ML_wrapper::getGlobalMlWrapper()->removeMgeLayerObject(text);
		m_text.reset();
		_ASSERT(text.use_count() == 1); //correct = last instance is local
	}
}
