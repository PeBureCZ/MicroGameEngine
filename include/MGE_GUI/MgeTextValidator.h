#pragma once
#include <memory>
#include <string_view>

class MgeTextValidator;

/**@brief Strategy used by MgeTextFrame to decide whether a typed character may be appended to the edited line.
@note A validator only filters input - it never changes, formats or colors the text.
@note Characters are UTF-16 code units (wchar_t). Characters above U+FFFF (surrogate pairs) are always rejected.
@note Derived validators override isCharAllowed() and/or isTextAllowed() (both allow everything by default);
a typed character passes only if both accept it. Callers use the public isAllowed() overloads.
@see MgeTextFrame::setValidator()*/
class MgeTextValidator
{
public:
	MgeTextValidator() = default;
	virtual ~MgeTextValidator() = default;

	/**@param newChar character typed by the user, e.g. L'x'
	@param currentText text of the edited line before newChar is appended*/
	[[nodiscard]] bool isAllowed(wchar_t newChar, std::wstring_view currentText) const noexcept;

	/**UTF-8 variant, e.g. isAllowed("x", "abc"); newChar_utf8 must contain exactly one character*/
	[[nodiscard]] bool isAllowed(std::string_view newChar_utf8, std::string_view currentText_utf8) const noexcept;

protected:
	MgeTextValidator(const MgeTextValidator&) = default;
	MgeTextValidator(MgeTextValidator&&) = default;
	MgeTextValidator& operator=(const MgeTextValidator&) = default;
	MgeTextValidator& operator=(MgeTextValidator&&) = default;

	/**rule for the typed character alone; newChar is never a surrogate code unit*/
	[[nodiscard]] virtual bool isCharAllowed(wchar_t /*newChar*/) const noexcept { return true; }
	/**rule for the whole edited line after the typed character is appended*/
	[[nodiscard]] virtual bool isTextAllowed(std::wstring_view /*candidateText*/) const noexcept { return true; }
};
