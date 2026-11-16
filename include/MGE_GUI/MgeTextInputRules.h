#pragma once
#include <functional>
#include <limits>
#include <string_view>
#include <vector>

#include "GlobalFunctions.h"

constexpr size_t UNLIMITED_DIGITS = (std::numeric_limits<size_t>::max)(); //parentheses: max() macro from <windows.h>

/**@brief Rule used by MgeTextFrame to decide whether a typed character may be appended to the edited (last) line.
@param newChar character typed by the user, never a surrogate code unit
@param candidateText edited line with newChar already appended (candidateText.back() == newChar)
@return true = the character is accepted
@note A rule only filters input - it never changes, formats or colors the text.
@note A rule must not throw.
@see MgeTextFrame::addInputRule(), mge::inputRules*/
using TextInputRule = std::function<bool(wchar_t newChar, std::wstring_view candidateText)>;

struct NumberRuleSettings
{
	mgeCore::DecimalSeparator m_separator = mgeCore::DecimalSeparator::DotAndComma;
	size_t m_maxDecimals = UNLIMITED_DIGITS;      //0 = integers only
	size_t m_maxIntegerDigits = UNLIMITED_DIGITS; //digits before the separator, must be > 0
	bool m_allowNegative = true;
};

/**Ready-made rules for MgeTextFrame::addInputRule(). App-specific rules are plain lambdas with the TextInputRule signature.*/
namespace mge::inputRules
{
	/**only listed characters pass, e.g. allowChars(L"+-*")*/
	[[nodiscard]] TextInputRule allowChars(std::wstring_view characters);
	/**only listed characters pass, e.g. allowChars("+-*")*/
	[[nodiscard]] TextInputRule allowChars(std::string_view characters_utf8);

	/**listed characters are rejected, everything else passes*/
	[[nodiscard]] TextInputRule denyChars(std::wstring_view characters);
	/**listed characters are rejected, everything else passes*/
	[[nodiscard]] TextInputRule denyChars(std::string_view characters_utf8);

	/**only characters in the inclusive range pass, e.g. charRange(L'0', L'9')*/
	[[nodiscard]] TextInputRule charRange(wchar_t first, wchar_t last);

	/**the edited line may have at most maxLength characters*/
	[[nodiscard]] TextInputRule maxLength(size_t maxLength);

	/**@brief The whole edited line must stay a valid number prefix.
	Format: [-]digits[separator digits], e.g. "-", "-12", "12,", "12.50". Leading zeros are allowed.
	@note The separator needs a digit before it ("-." and "." are rejected).
	@note If the line already contains invalid text (e.g. set by code), no typed character is accepted.
	@note The text is not normalized: "12," stays "12," - parse it with mgeCore::fromStringToDouble().*/
	[[nodiscard]] TextInputRule number(const NumberRuleSettings& settings = NumberRuleSettings());

	/**the character passes if at least one of the rules accepts it, e.g. anyOf({ charRange(L'a', L'z'), allowChars(L"_") })*/
	[[nodiscard]] TextInputRule anyOf(std::vector<TextInputRule> rules);
}
