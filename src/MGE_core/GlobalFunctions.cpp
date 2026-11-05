#include "GlobalFunctions.h"

#include <deque>
#include <atomic>
#include <cmath>
#include <string>
#include <string_view>
#include <system_error>
#include <charconv>

static bool mainThreadRun = false;
static std::thread::id mainThreadId;
constexpr int64_t DEFAULT_Z_POSITION = 1024;
static std::atomic<int64_t> defaultZPosition = DEFAULT_Z_POSITION;

namespace mgeCore
{
    [[nodiscard]] std::wstring getWExecutablePath(bool includeExeName)
    {
        wchar_t buffer[MAX_PATH];
        GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		std::wstring path(buffer);
        if (!includeExeName)
        {
            size_t pos = path.find_last_of(L"\\/");
            if (pos != std::wstring::npos)
                path = path.substr(0, pos);
        }
		return path;
    }

    [[nodiscard]] std::string getExecutablePath(bool includeExeName)
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);

        std::string path(buffer);

        if (!includeExeName)
        {
            size_t pos = path.find_last_of("\\/");
            if (pos != std::string::npos)
                path = path.substr(0, pos);
        }

        return path;
    }

    void setThisThreadAsMain()
    {
        mainThreadRun = true;
        mainThreadId = std::this_thread::get_id();
#ifdef _DEBUG
        static size_t usedMainThreadCounter = 0;
        _ASSERT(usedMainThreadCounter++ == 0); // "set main thread only once!"
#endif // _DEBUG
    }

    [[nodiscard]] bool mainThreadIsSet() noexcept
    {
        return mainThreadRun;
    }

    [[nodiscard]] bool isMainThread() noexcept
    {
        return std::this_thread::get_id() == mainThreadId;
    }

    [[nodiscard]] std::string toUTF8(const std::wstring& wstr)
    {
        if (wstr.empty())
            return {};

        int size = WideCharToMultiByte
            (
                CP_UTF8,
                0,
                wstr.data(),
                static_cast<int>(wstr.size()),
                nullptr,
                0,
                nullptr,
                nullptr
            );

        if (size <= 0)
            return {};

        std::string result(size, '\0');

        int written = WideCharToMultiByte
            (
                CP_UTF8,
                0,
                wstr.data(),
                static_cast<int>(wstr.size()),
                result.data(),
                size,
                nullptr,
                nullptr
            );

        if (written <= 0 || written != size)
            return {};

        return result;
    }

    [[nodiscard]] std::wstring fromUTF8(const std::string& str)
    {
        if (str.empty())
            return {};

        int size = MultiByteToWideChar
            (
                CP_UTF8,
                0,
                str.data(),
                static_cast<int>(str.size()),
                nullptr,
                0
            );

        if (size <= 0)
            return {};

        std::wstring result(size, L'\0');

        int written = MultiByteToWideChar
            (
                CP_UTF8,
                0,
                str.data(),
                static_cast<int>(str.size()),
                result.data(),
                size
            );

        if (written <= 0 || written != size)
            return {};

        return result;
    }

    void setDefaultZPosition(int64_t newDefaultValue)
    {
        defaultZPosition.store(newDefaultValue);
    }

    int64_t getDefaultZPosition(ZPositionAdditionMode additionMode)
    {
        return defaultZPosition.load() + static_cast<int64_t>(additionMode);
    }

    [[nodiscard]] std::string formatNumber(double value, DecimalSeparator separator, size_t maxDecimals ) noexcept
    {
        _ASSERT(separator != DecimalSeparator::DotAndComma);
        try
        {
            if (!std::isfinite(value))
            {
                if (std::isnan(value))
                    return "nan";

                return std::signbit(value) ? "-inf" : "inf";
            }

            char buffer[128];

            const auto [ptr, ec] = std::to_chars(std::begin(buffer), std::end(buffer), value,
                std::chars_format::fixed,
                static_cast<int>((std::min)(maxDecimals, size_t{ 100 })));

            if (ec != std::errc{})
                return {};

            std::string result(buffer, ptr);

            const auto dotPos = result.find('.');

            if (dotPos != std::string::npos)
            {
                while (!result.empty() && result.back() == '0')
                    result.pop_back();

                if (!result.empty() && result.back() == '.')
                    result.pop_back();
            }

            if (separator == DecimalSeparator::Comma)
            {
                const auto pos = result.find('.');
                if (pos != std::string::npos)
                    result[pos] = ',';
            }

            return result;
        }
        catch (...)
        {
            return {};
        }
    }

    [[nodiscard]] double fromStringToDouble(const std::string& text, DecimalSeparator allowSeparator) noexcept
    {
        if (text.empty())
            return 0.0;

        double value = 0.0;


        try
        {
            std::string normalized;
            normalized.reserve(text.size());

            bool hasDot = false;
            bool hasComma = false;

            for (const char c : text)
            {
                if (c == '.')
                {
                    if (allowSeparator == DecimalSeparator::Comma)
                        return 0.0;

                    if (hasDot)
                        return 0.0;

                    hasDot = true;
                    normalized.push_back('.');
                }
                else if (c == ',')
                {
                    if (allowSeparator == DecimalSeparator::Dot)
                        return 0.0;

                    if (hasComma)
                        return 0.0;

                    hasComma = true;
                    normalized.push_back('.');
                }
                else
                    normalized.push_back(c);
            }

            // A number cannot contain both decimal separators.
            if (hasDot && hasComma)
                return 0.0;


            const char* begin = normalized.data();
            const char* end = begin + normalized.size();

            const auto [ptr, ec] = std::from_chars(begin, end, value);

            if (ec != std::errc{} || ptr != end || !std::isfinite(value))
                return 0.0;
        }
        catch (...)
        {
            _ASSERT(false); // unexpected exception
        }

        return value;
    }

    bool isControlCharacter(std::string_view utf8Char) noexcept
    {
        if (utf8Char.empty())
            return true;
        const auto firstByte = static_cast<unsigned char>(utf8Char.front());
        return firstByte < 0x20 || firstByte == 0x7F;
    }

}
