#pragma once
#include <limits>
#include <memory>
#include <string_view>

#include "GlobalFunctions.h"
#include "MgeTextValidator.h"

constexpr size_t UNLIMITED_DIGITS = (std::numeric_limits<size_t>::max)(); //parentheses: max() macro from <windows.h>

struct NumberValidatorSettings
{
	mgeCore::DecimalSeparator m_separator = mgeCore::DecimalSeparator::DotAndComma;
	size_t m_maxDecimals = UNLIMITED_DIGITS;      //0 = integers only
	size_t m_maxIntegerDigits = UNLIMITED_DIGITS; //digits before the separator, must be > 0
	bool m_allowNegative = true;
};

class MgeNumberValidator;

namespace mge
{
	using NumberValidator = std::shared_ptr<MgeNumberValidator>;
	NumberValidator createNumberValidator(const NumberValidatorSettings& settings = NumberValidatorSettings());
}

/**@brief Accepts typed characters only while the whole edited line stays a valid number prefix.
Format: [-]digits[separator digits], e.g. "-", "-12", "12,", "12.50". Leading zeros are allowed.
@note The separator needs a digit before it ("-." and "." are rejected).
@note If the line already contains invalid text (e.g. set by code), no typed character is accepted.
@note The validator does not normalize the text: "12," stays "12," - parse it with mgeCore::fromStringToDouble().*/
class MgeNumberValidator : public MgeTextValidator
{
public:
	explicit MgeNumberValidator(const NumberValidatorSettings& settings = NumberValidatorSettings()) noexcept;

	[[nodiscard]] const NumberValidatorSettings& getSettings() const noexcept;
	void setSettings(const NumberValidatorSettings& newSettings) noexcept;

protected:
	[[nodiscard]] bool isTextAllowed(std::wstring_view candidateText) const noexcept override;

private:
	NumberValidatorSettings m_settings;

	[[nodiscard]] std::wstring_view getSeparators() const noexcept;
	[[nodiscard]] bool isSeparator(wchar_t character) const noexcept;
};
