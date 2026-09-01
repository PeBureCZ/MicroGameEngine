#pragma once

#include <string>

namespace mgeVersion
{
	constexpr unsigned int MAJOR = 0;
	constexpr unsigned int MINOR = 1;
	constexpr unsigned int PATCH = 1;
	constexpr std::string_view VERSION = "0.1.1";
	constexpr std::wstring_view VERSION_W = L"0.1.1";

	inline static std::string getVersion()
	{
		return std::string(VERSION);
	}

	inline static std::wstring getVersionW()
	{
		return std::wstring(VERSION_W);
	}
}

