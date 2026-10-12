#pragma once
#include <array>
#include <string>
#include "GlobalFunctions.h"

inline const char* UNDEFINED_TEXTURE_PATH = "undefined";

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
	TextureId(std::string usedPath = std::string(UNDEFINED_TEXTURE_PATH))
		: m_path(std::move(usedPath))
	{

	}

	TextureId(std::wstring usedPath)
		: m_path(mgeCore::toUTF8(usedPath))
	{

	}

		bool operator == (const TextureId& other) const
	{
		return m_path == other.m_path;
	}

	bool operator == (const std::string& other) const
	{
		return m_path == other;
	}

	bool operator == (const char* other) const
	{
		return m_path == other;
	}

	const std::string& getPath() const
	{
		return m_path;
	}
private:
	std::string m_path;
};

namespace std
{ //allow create hash of TextureId for maps or unordered maps
	template<>
	struct hash<TextureId>
	{
		size_t operator()(const TextureId& id) const noexcept
		{
			return std::hash<std::string>{}(id.getPath());
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