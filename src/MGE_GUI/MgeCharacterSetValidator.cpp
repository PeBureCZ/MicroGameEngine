#include "MgeCharacterSetValidator.h"

#include <algorithm>
#include <string>

#include "GlobalFunctions.h"

MgeCharacterSetValidator::MgeCharacterSetValidator(CharacterSetMode mode) noexcept
	: m_mode(mode)
{
}

void MgeCharacterSetValidator::addCharacter(wchar_t character)
{
	addRange(character, character);
}

void MgeCharacterSetValidator::addCharacters(std::wstring_view characters)
{
	for (const wchar_t character : characters)
		addCharacter(character);
}

void MgeCharacterSetValidator::addCharacters(std::string_view characters_utf8)
{
	addCharacters(mgeCore::fromUTF8(std::string(characters_utf8)));
}

void MgeCharacterSetValidator::addRange(wchar_t first, wchar_t last)
{
	_ASSERT(first <= last);
	if (first > last)
		return;

	_ASSERT(first > 0xDFFF || last < 0xD800); //characters above U+FFFF are not supported
	m_ranges.push_back(CharacterRange{ first, last });
}

void MgeCharacterSetValidator::clear() noexcept
{
	m_ranges.clear();
}

CharacterSetMode MgeCharacterSetValidator::getMode() const noexcept
{
	return m_mode;
}

void MgeCharacterSetValidator::setMode(CharacterSetMode newMode) noexcept
{
	m_mode = newMode;
}

bool MgeCharacterSetValidator::isCharAllowed(wchar_t newChar) const noexcept
{
	const bool isListed = contains(newChar);
	return (m_mode == CharacterSetMode::Allow) ? isListed : !isListed;
}

bool MgeCharacterSetValidator::contains(wchar_t character) const noexcept
{
	return std::any_of(m_ranges.begin(), m_ranges.end(),
		[character](const CharacterRange& range) { return character >= range.m_first && character <= range.m_last; });
}

namespace mge
{
	CharacterSetValidator createCharacterSetValidator(std::wstring_view characters, CharacterSetMode mode)
	{
		auto newValidator = std::make_shared<MgeCharacterSetValidator>(mode);
		newValidator->addCharacters(characters);
		return newValidator;
	}

	CharacterSetValidator createCharacterSetValidator(std::string_view characters_utf8, CharacterSetMode mode)
	{
		auto newValidator = std::make_shared<MgeCharacterSetValidator>(mode);
		newValidator->addCharacters(characters_utf8);
		return newValidator;
	}
}
