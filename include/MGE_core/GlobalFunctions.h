#pragma once
#include <string>
#include <string_view>
#include <filesystem>
#include <windows.h>
#include <thread>
#include <functional>

#define MAIN_THREAD_GUARD _ASSERT(mgeCore::mainThreadIsSet() && mgeCore::isMainThread())

namespace mgeCore
{
	/**folder of the running exe (or the exe itself), any length, any characters*/
	[[nodiscard]] std::filesystem::path getExecutableLocation(bool includeExeName = false);
	[[nodiscard]] std::wstring getWExecutablePath(bool includeExeName = false);
	//UTF-8 (not ANSI)
	[[nodiscard]] std::string getExecutablePath(bool includeExeName = false);

	void setThisThreadAsMain();
	[[nodiscard]] bool mainThreadIsSet() noexcept;
	[[nodiscard]] bool isMainThread() noexcept;
    [[nodiscard]] std::string toUTF8(const std::wstring& wstr);
	[[nodiscard]] std::wstring fromUTF8(const std::string& str);
	void setDefaultZPosition(int64_t newDefaultValue);

	enum ZPositionAdditionMode : int64_t
	{
		Decrease = -100,
		None = 0,
		Text = 100,
	};

	[[nodiscard]] int64_t getDefaultZPosition(ZPositionAdditionMode additionMode = ZPositionAdditionMode::None);

	enum class DecimalSeparator
	{
		Dot,
		Comma,
		DotAndComma
	};
	[[nodiscard]] std::string formatNumber(double value, DecimalSeparator separator, size_t maxDecimals = 4) noexcept;

	[[nodiscard]] double fromStringToDouble(const std::string& text, DecimalSeparator allowSeparator) noexcept;

	// true for C0 controls (\0..\x1F: \b, \t, \n, \r, Esc, ...) and DEL (\x7F, Ctrl+Backspace)
	[[nodiscard]] bool isControlCharacter(std::string_view utf8Char) noexcept;
}


