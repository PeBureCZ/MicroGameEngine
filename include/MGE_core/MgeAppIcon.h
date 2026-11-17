#pragma once

/*
* Application icon shared by the .exe file and the window.
*
* The game (.exe) project adds app.rc (next to its .vcxproj) with one line:
*     MGE_APP_ICON ICON "app.ico"
* MGE_APP_ICON must not be #defined anywhere the .rc sees, so the resource is stored by the text name "MGE_APP_ICON".
*
* The engine loads this icon onto the window automatically; without the resource the default icon stays.
*/

namespace mgeCore
{
	/**resource name used in app.rc (case insensitive)*/
	constexpr const wchar_t* MGE_APP_ICON_NAME = L"MGE_APP_ICON";

	/**sets the big (Alt+Tab, taskbar) and small (title bar) icon of the window from an ICON resource of the running exe
	* windowHandle = HWND, returns false when the exe has no such resource*/
	bool setWindowIconFromResource(void* windowHandle, const wchar_t* resourceName = MGE_APP_ICON_NAME);
}
