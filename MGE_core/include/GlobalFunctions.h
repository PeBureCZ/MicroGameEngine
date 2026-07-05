#pragma once
#include <string>
#include <windows.h>
#include <thread>

#define MAIN_THREAD_GUARD _ASSERT(tsmCore::mainThreadIsSet() && tsmCore::isMainThread())

namespace tsmCore
{
    [[nodiscard]] std::string getExecutablePath(bool includeExeName = false);
	void setThisThreadAsMain();
	[[nodiscard]] bool mainThreadIsSet();
	[[nodiscard]] bool isMainThread();
    [[nodiscard]] std::string toUTF8(const std::wstring& wstr);
	[[nodiscard]] std::wstring fromUTF8(const std::string& str);
}


