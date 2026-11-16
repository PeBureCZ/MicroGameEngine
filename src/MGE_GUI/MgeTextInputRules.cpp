#include "MgeTextInputRules.h"

#include <algorithm>
#include <string>

namespace
{
	[[nodiscard]] bool isDigit(wchar_t character) noexcept
	{
		return character >= L'0' && character <= L'9'; //ASCII digits only
	}

	/**moves index behind the digits starting at index, returns their count*/
	[[nodiscard]] size_t skipDigits(std::wstring_view text, size_t& index) noexcept
	{
		const size_t start = index;
		while (index < text.size() && isDigit(text[index]))
			++index;
		return index - start;
	}

	[[nodiscard]] std::wstring_view getSeparators(mgeCore::DecimalSeparator separator) noexcept
	{
		switch (separator)
		{
		case mgeCore::DecimalSeparator::Dot:
			return L".";
		case mgeCore::DecimalSeparator::Comma:
			return L",";
		case mgeCore::DecimalSeparator::DotAndComma:
			return L".,";
		default:
			_ASSERT(false); //unknown separator
			return L"";
		}
	}

	[[nodiscard]] bool isNumberPrefix(std::wstring_view text, const NumberRuleSettings& settings) noexcept
	{
		size_t index = 0;

		if (index < text.size() && text[index] == L'-')
		{
			if (!settings.m_allowNegative)
				return false;
			++index;
		}

		const size_t integerDigits = skipDigits(text, index);
		if (integerDigits > settings.m_maxIntegerDigits)
			return false;

		if (index == text.size()) //"-" or "12"
			return true;

		const bool isSeparator = getSeparators(settings.m_separator).find(text[index]) != std::wstring_view::npos;
		if (integerDigits == 0 || settings.m_maxDecimals == 0 || !isSeparator)
			return false;
		++index;

		const size_t decimals = skipDigits(text, index);
		return index == text.size() && decimals <= settings.m_maxDecimals;
	}

	[[nodiscard]] TextInputRule characterListRule(std::wstring_view characters, bool isAllowList)
	{
		_ASSERT(std::none_of(characters.begin(), characters.end(),
			[](wchar_t character) { return character >= 0xD800 && character <= 0xDFFF; })); //characters above U+FFFF are not supported

		return [listedCharacters = std::wstring(characters), isAllowList](wchar_t newChar, std::wstring_view) noexcept
		{
			const bool isListed = listedCharacters.find(newChar) != std::wstring::npos;
			return isListed == isAllowList;
		};
	}
}

namespace mge::inputRules
{
	TextInputRule allowChars(std::wstring_view characters)
	{
		return characterListRule(characters, true /*isAllowList*/);
	}

	TextInputRule allowChars(std::string_view characters_utf8)
	{
		return allowChars(mgeCore::fromUTF8(std::string(characters_utf8)));
	}

	TextInputRule denyChars(std::wstring_view characters)
	{
		return characterListRule(characters, false /*isAllowList*/);
	}

	TextInputRule denyChars(std::string_view characters_utf8)
	{
		return denyChars(mgeCore::fromUTF8(std::string(characters_utf8)));
	}

	TextInputRule charRange(wchar_t first, wchar_t last)
	{
		_ASSERT(first <= last);
		return [first, last](wchar_t newChar, std::wstring_view) noexcept { return newChar >= first && newChar <= last; };
	}

	TextInputRule maxLength(size_t maxLength)
	{
		return [maxLength](wchar_t, std::wstring_view candidateText) noexcept { return candidateText.size() <= maxLength; };
	}

	TextInputRule number(const NumberRuleSettings& settings)
	{
		_ASSERT(settings.m_maxIntegerDigits > 0); //no number could be typed
		return [settings](wchar_t, std::wstring_view candidateText) noexcept { return isNumberPrefix(candidateText, settings); };
	}

	TextInputRule anyOf(std::vector<TextInputRule> rules)
	{
		_ASSERT(std::all_of(rules.begin(), rules.end(), [](const TextInputRule& rule) { return static_cast<bool>(rule); }));

		return [rules = std::move(rules)](wchar_t newChar, std::wstring_view candidateText)
		{
			return std::any_of(rules.begin(), rules.end(),
				[newChar, candidateText](const TextInputRule& rule) { return rule && rule(newChar, candidateText); });
		};
	}
}
