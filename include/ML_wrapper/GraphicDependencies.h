#pragma once
#include <array>
#include <string>
#include "GlobalFunctions.h"

using VerticesId = uintptr_t;

enum class GuiAlign : size_t
{
	TopLeft,
	TopCenter,
	TopRight,
	MiddleLeft,
	MiddleCenter,
	MiddleRight,
	BottomLeft,
	BottomCenter,
	BottomRight
};

struct TextureId
{
public:
	TextureId(std::string usedPath_utf8 = "")
		: m_path(mgeCore::fromUTF8(usedPath_utf8))
	{}

	TextureId(std::wstring usedPath)
		: m_path(usedPath)
	{}

	bool operator == (const TextureId& other) const
	{
		return m_path == other.m_path;
	}

	bool operator == (const std::wstring& other) const
	{
		return m_path == other;
	}

	bool operator == (const wchar_t* other) const
	{
		return m_path == other;
	}

	const std::wstring& getPath() const
	{
		return m_path;
	}

	[[nodiscard]] std::filesystem::path getFilesystemPath() const
	{
		return std::filesystem::path(m_path);
	}

private:
	std::wstring m_path;
};

namespace std
{ //allow create hash of TextureId for maps or unordered maps
	template<>
	struct hash<TextureId>
	{
		size_t operator()(const TextureId& id) const noexcept
		{
			return std::hash<std::wstring>{}(id.getPath());
		}
	};
}

enum GraphicItemLayer : size_t
{
	UNRENDERED_LAYER = 0,
	BOTTOM_LAYER,
	GUI_LAYER = 10000,
	WINDOW_LAYER = 10001,
	DEFAULT_LAYER = 120000,
	TOP_LAYER = 2000000,
};