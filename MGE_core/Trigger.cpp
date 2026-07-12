#include "Trigger.h"

namespace CollisionSystem
{
	static tsmType::Size<size_t> g_cellSize = tsmType::Size<size_t>(5000, 5000);

	//use Actor position and cell size to determine which cell the actor belongs to,
	// then use that cell id to store the collision component of the actor in the corresponding cell of the collision channel
	CellId getCellIdForPosition(const FPoint& position) noexcept
	{
		if (g_cellSize.width == 0 || g_cellSize.height == 0)
		{
			_ASSERT(false); //invalid cell size
			g_cellSize = tsmType::Size<size_t>((std::max)(g_cellSize.width, (size_t)1), (std::max)(g_cellSize.height, (size_t)1));
		}

		return CellId(static_cast<int>(position.x / (float)g_cellSize.width), static_cast<int>(position.y / (float)g_cellSize.height));
	}

	void setCellSize(tsmType::Size<size_t> newCellSize) noexcept
	{
		_ASSERT(newCellSize.width > 0 && newCellSize.height > 0); //cell size should be positive
		if (newCellSize.width == 0 || newCellSize.height == 0)
		{
			_ASSERT(false); //invalid cell size
			newCellSize = tsmType::Size<size_t>((std::max)(newCellSize.width, (size_t)1), (std::max)(newCellSize.height, (size_t)1));
		}
		g_cellSize = newCellSize;
	}

	tsmType::Size<size_t> getCellSize() noexcept
	{
		return g_cellSize;
	}
}