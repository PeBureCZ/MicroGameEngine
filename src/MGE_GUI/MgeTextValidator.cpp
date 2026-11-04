#include "MgeTextValidator.h"

#include <string>

#include "GlobalFunctions.h"

namespace
{
	[[nodiscard]] bool isSurrogate(wchar_t character) noexcept
	{
		return character >= 0xD800 && character <= 0xDFFF;
	}
}

bool MgeTextValidator::isAllowed(wchar_t newChar, std::wstring_view currentText) const noexcept
{
	if (isSurrogate(newChar)) //half of a character above U+FFFF - not supported
		return false;

	if (!isCharAllowed(newChar))
		return false;

	try
	{
		std::wstring candidateText;
		candidateText.reserve(currentText.size() + 1);
		candidateText.append(currentText);
		candidateText.push_back(newChar);
		return isTextAllowed(candidateText);
	}
	catch (...)
	{
		_ASSERT(false); //allocation failed
		return false;
	}
}

bool MgeTextValidator::isAllowed(std::string_view newChar_utf8, std::string_view currentText_utf8) const noexcept
{
	try
	{
		const std::wstring newChar = mgeCore::fromUTF8(std::string(newChar_utf8));
		if (newChar.size() != 1) //empty, more characters, or a character above U+FFFF (surrogate pair)
			return false;

		const std::wstring currentText = mgeCore::fromUTF8(std::string(currentText_utf8));
		return isAllowed(newChar.front(), currentText);
	}
	catch (...)
	{
		_ASSERT(false); //allocation failed
		return false;
	}
}
