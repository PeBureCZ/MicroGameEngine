#pragma once

#include <string>

namespace mgeVersion
{
	constexpr unsigned int MAJOR = 0;
	constexpr unsigned int MINOR = 1;
	constexpr unsigned int PATCH = 3;
	constexpr std::string_view VERSION = "0.1.3";
	constexpr std::wstring_view VERSION_W = L"0.1.3";

	inline static std::string getVersion()
	{
		return std::string(VERSION);
	}

	inline static std::wstring getVersionW()
	{
		return std::wstring(VERSION_W);
	}
}

