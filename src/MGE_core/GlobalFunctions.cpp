#include "GlobalFunctions.h"

#include <deque>
#include <atomic>

static bool mainThreadRun = false;
static std::thread::id mainThreadId;
constexpr int64_t DEFAULT_Z_POSITION = 1024;
static std::atomic<int64_t> defaultZPosition = DEFAULT_Z_POSITION;

namespace mgeCore
{
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

    [[nodiscard]] bool mainThreadIsSet()
    {
        return mainThreadRun;
    }

    [[nodiscard]] bool isMainThread()
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

    int64_t getDefaultZPosition()
    {
        return defaultZPosition.load();
    }
}