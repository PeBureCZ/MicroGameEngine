#pragma once
#include <array>
#include <string>

inline const char* UNDEFINED_TEXTURE_PATH = "undefined";

using VerticesId = uintptr_t;

struct TextureId
{
public:
	TextureId(std::string usedPath = std::string(UNDEFINED_TEXTURE_PATH))
		: path(std::move(usedPath))
	{

	}
	std::string path;

	bool operator == (const TextureId& other) const
	{
		return path == other.path;
	}

	bool operator == (const std::string& other) const
	{
		return path == other;
	}

	bool operator == (const char* other) const
	{
		return path == other;
	}

	const std::string getPath() const
	{
		return path;
	}
};

namespace std
{ //allow create hash of TextureId for maps or unordered maps
	template<>
	struct hash<TextureId>
	{
		size_t operator()(const TextureId& id) const noexcept
		{
			return std::hash<std::string>{}(id.path);
		}
	};
}

enum GraphicItemLayer : size_t
{
	UNRENDERED_LAYER = 0,
	BOTTOM_LAYER,
	GUI_LAYER = 10000,
	DEFAULT_LAYER = 120000,
	TOP_LAYER = 2000000,
};