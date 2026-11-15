#pragma once

#include <string>

namespace mgeVersion
{
	constexpr std::string_view VERSION = "0.3.0";
	constexpr std::wstring_view VERSION_W = L"0.3.0";

	inline static std::string getVersion()
	{
		return std::string(VERSION);
	}

	inline static std::wstring getVersionW()
	{
		return std::wstring(VERSION_W);
	}
}

