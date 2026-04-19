#pragma once
#include <string>
#include <windows.h>
#include <thread>

#define MAIN_THREAD_GUARD _ASSERT(tsmCore::mainThreadIsSet() && tsmCore::isMainThread())

namespace tsmCore
{
    std::string getExecutablePath(bool includeExeName = false);
	void setThisThreadAsMain();
	bool mainThreadIsSet();
	bool isMainThread();
}


