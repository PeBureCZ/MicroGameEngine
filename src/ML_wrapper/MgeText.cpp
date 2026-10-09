#include "MgeText.h"

#include "GlobalFunctions.h"
#include "layerDefinition.h"

#include "MlWrapper.h"

//static void normalizeTextHeight(sf::Text& text)
//{
//	auto charSize = text.getCharacterSize();
//	auto& glyph1 = text.getFont().getGlyph('x', charSize, false);
//	auto& glyph2 = text.getFont().getGlyph('H', charSize, false);
//	auto topShift = glyph2.textureRect.size.y - glyph1.textureRect.size.y;
//	text.setOrigin(sf::Vector2f(text.getOrigin().x, text.getOrigin().y + topShift));
//}

static void normalizeTextOrigin(sf::Text& text)
{
	//change the origin to left-top corner of the text, so that the position is the left-top corner of the text
	const auto bounds = text.getLocalBounds();
	text.setOrigin({bounds.position.x, bounds.position.y});
}

MgeText::MgeText(std::string newText, unsigned int characterSize_pxls, const FPoint& position, bool bold, size_t layer)
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
	normalizeTextOrigin(sfText);
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

void MgeText::setColor(const MgeColor& newColor)
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

void MgeText::setText(const std::string& newText)
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			const auto getAbsolutePos = getAbsolutePosition();
			const sf::String sfString(newText);
			auto& sfText = std::get<sf::Text>(text->data);

			const auto oldBounds = sfText.getLocalBounds();
			sfText.setString(sfString);
			const auto newBounds = sfText.getLocalBounds();

			const int horizontalOffset = static_cast<int>((newBounds.size.x - oldBounds.size.x) * 0.5f);

			setAbsolutePosition(IPoint(getAbsolutePos.x - horizontalOffset, getAbsolutePos.y));
		}
		else
		{
			_ASSERT(false); //wrong type
		}
	}
	else
	{
		_ASSERT(false);
	}
}

void MgeText::setFontSize(unsigned int newSize)
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			auto& sfText = std::get<sf::Text>(text->data);
			sfText.setCharacterSize(newSize);
			normalizeTextOrigin(sfText);
		}
		else
		{
			_ASSERT(false); //wrong type
		}
	}
}

unsigned int MgeText::getFontSize() const
{
	_ASSERT(!m_text.expired());
	if (auto text = m_text.lock())
	{
		if (std::holds_alternative<sf::Text>(text->data))
		{
			auto& sfText = std::get<sf::Text>(text->data);
			return sfText.getCharacterSize();
		}
		else
		{
			_ASSERT(false); //wrong type
			return 0;
		}
	}
	else
		return 0;
}

std::string MgeText::getText() const
{
	const auto& textObject = m_text.lock();

	if (!textObject || !std::holds_alternative<sf::Text>(textObject->data))
		return {};

	const sf::Text& text = std::get<sf::Text>(textObject->data);
	const sf::U8String utf8 = text.getString().toUtf8();
	std::string result(utf8.begin(), utf8.end());
	return result;
}

MgeColor MgeText::getColor() const noexcept
{
	if (m_text.expired())
		return MgeColor();
	else if (std::holds_alternative<sf::Text>(m_text.lock()->data))
	{
		auto& sfText = std::get<sf::Text>(m_text.lock()->data);
		auto col = sfText.getFillColor();
		return MgeColor(col.r, col.g, col.b, col.a);
	}
	else
	{
		_ASSERT(false); //wrong type
		return MgeColor();
	}
}

bool MgeText::isVisible() const noexcept
{
	return m_isVisible;
}

bool MgeText::isBold() const noexcept
{
	if (m_text.expired())
		return false;
	else if (std::holds_alternative<sf::Text>(m_text.lock()->data))
	{
		return std::get<sf::Text>(m_text.lock()->data).getStyle() & sf::Text::Bold;
	}
	else
	{
		_ASSERT(false); //wrong type
		return false;
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

GuiAlign MgeText::getAlign() const noexcept
{
	return m_align;
}

void MgeText::setAlign(GuiAlign align) noexcept
{
	m_align = align;
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
