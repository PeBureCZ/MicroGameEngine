#pragma once
#include <memory>
#include <string_view>
#include <vector>

#include "MgeTextValidator.h"

enum class CharacterSetMode
{
	Allow, //only listed characters pass
	Deny   //listed characters are rejected, everything else passes
};

class MgeCharacterSetValidator;

namespace mge
{
	using CharacterSetValidator = std::shared_ptr<MgeCharacterSetValidator>;

	/**@param characters listed characters, e.g. L"+-*"*/
	CharacterSetValidator createCharacterSetValidator(std::wstring_view characters = L"", CharacterSetMode mode = CharacterSetMode::Allow);
	/**@param characters_utf8 listed characters, e.g. "+-*"*/
	CharacterSetValidator createCharacterSetValidator(std::string_view characters_utf8, CharacterSetMode mode = CharacterSetMode::Allow);
}

/**@brief Validator with an allow list or a deny list of characters and character ranges.
@note The list can be changed after the validator is set to a frame; the frame always uses the current list.*/
class MgeCharacterSetValidator : public MgeTextValidator
{
public:
	explicit MgeCharacterSetValidator(CharacterSetMode mode = CharacterSetMode::Allow) noexcept;

	void addCharacter(wchar_t character);
	void addCharacters(std::wstring_view characters);
	void addCharacters(std::string_view characters_utf8);
	/**inclusive range, e.g. addRange(L'0', L'9')*/
	void addRange(wchar_t first, wchar_t last);
	void clear() noexcept;

	[[nodiscard]] CharacterSetMode getMode() const noexcept;
	void setMode(CharacterSetMode newMode) noexcept;

protected:
	[[nodiscard]] bool isCharAllowed(wchar_t newChar) const noexcept override;

private:
	struct CharacterRange
	{
		wchar_t m_first = 0;
		wchar_t m_last = 0;
	};

	CharacterSetMode m_mode = CharacterSetMode::Allow;
	std::vector<CharacterRange> m_ranges;

	[[nodiscard]] bool contains(wchar_t character) const noexcept;
};
