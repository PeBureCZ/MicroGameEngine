
#include "GlobalFunctions.h"

static bool mainThreadRun = false;
static std::thread::id mainThreadId;

namespace tsmCore
{
    std::string getExecutablePath(bool includeExeName)
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
    bool mainThreadIsSet()
    {
        return mainThreadRun;
    }
    bool isMainThread()
    {
        return std::this_thread::get_id() == mainThreadId;
    }
}