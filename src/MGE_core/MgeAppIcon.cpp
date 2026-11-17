#include "MgeAppIcon.h"

#include <windows.h>
#include <crtdbg.h>
#include <string>

namespace
{
	/**module = the running exe (engine is a static lib linked into it), icon at the system size for that kind*/
	HICON loadIconResource(const wchar_t* resourceName, int widthMetric, int heightMetric)
	{
		return static_cast<HICON>(LoadImageW(GetModuleHandleW(nullptr), resourceName, IMAGE_ICON,
			GetSystemMetrics(widthMetric), GetSystemMetrics(heightMetric), LR_DEFAULTCOLOR));
	}
}

namespace mgeCore
{
	bool setWindowIconFromResource(void* windowHandle, const wchar_t* resourceName)
	{
		const HWND hwnd = static_cast<HWND>(windowHandle);
		_ASSERT(hwnd && resourceName);
		if (!hwnd || !resourceName)
			return false;

		//loaded once and kept for the whole process: the window is recreated on fullscreen switch and WM_SETICON does not take ownership
		static std::wstring loadedResourceName;
		static bool isLoaded = false;
		static HICON bigIcon = nullptr;
		static HICON smallIcon = nullptr;
		if (!isLoaded || _wcsicmp(loadedResourceName.c_str(), resourceName) != 0)
		{
			if (bigIcon)
				DestroyIcon(bigIcon);
			if (smallIcon)
				DestroyIcon(smallIcon);
			bigIcon = loadIconResource(resourceName, SM_CXICON, SM_CYICON);
			smallIcon = loadIconResource(resourceName, SM_CXSMICON, SM_CYSMICON);
			loadedResourceName = resourceName;
			isLoaded = true;
		}

		if (!bigIcon && !smallIcon)
			return false; //exe has no such icon, the default one stays

		if (bigIcon)
			SendMessageW(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(bigIcon));
		if (smallIcon)
			SendMessageW(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(smallIcon));
		return true;
	}
}
