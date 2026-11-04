#include "MgeNumberValidator.h"

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
}

MgeNumberValidator::MgeNumberValidator(const NumberValidatorSettings& settings) noexcept
{
	setSettings(settings);
}

const NumberValidatorSettings& MgeNumberValidator::getSettings() const noexcept
{
	return m_settings;
}

void MgeNumberValidator::setSettings(const NumberValidatorSettings& newSettings) noexcept
{
	_ASSERT(newSettings.m_maxIntegerDigits > 0); //no number could be typed
	m_settings = newSettings;
}

bool MgeNumberValidator::isTextAllowed(std::wstring_view candidateText) const noexcept
{
	size_t index = 0;

	if (index < candidateText.size() && candidateText[index] == L'-')
	{
		if (!m_settings.m_allowNegative)
			return false;
		++index;
	}

	const size_t integerDigits = skipDigits(candidateText, index);
	if (integerDigits > m_settings.m_maxIntegerDigits)
		return false;

	if (index == candidateText.size()) //"-" or "12"
		return true;

	if (integerDigits == 0 || m_settings.m_maxDecimals == 0 || !isSeparator(candidateText[index]))
		return false;
	++index;

	const size_t decimals = skipDigits(candidateText, index);
	return index == candidateText.size() && decimals <= m_settings.m_maxDecimals;
}

std::wstring_view MgeNumberValidator::getSeparators() const noexcept
{
	switch (m_settings.m_separator)
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

bool MgeNumberValidator::isSeparator(wchar_t character) const noexcept
{
	return getSeparators().find(character) != std::wstring_view::npos;
}

namespace mge
{
	NumberValidator createNumberValidator(const NumberValidatorSettings& settings)
	{
		return std::make_shared<MgeNumberValidator>(settings);
	}
}
