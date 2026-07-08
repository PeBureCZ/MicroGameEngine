#pragma once

#include <vector>
#include <unordered_map>

#include "Trigger.h"

template <typename T>
class CollisionChannel
{
public:
	CollisionChannel(int id)
		: channelId(id)
	{
	}

	[[nodiscard]] int getChannelId() const noexcept
	{
		return channelId;
	}

	void addElement(std::shared_ptr<CollisionComponent<T>> element)
	{
		auto& usedPosition = element->getOrigin();
		auto cellId = CollisionSystem::getCellIdForPosition(usedPosition);

#ifdef _DEBUG
		auto it = cellsOfCollisions.find(cellId);
		if (it != cellsOfCollisions.end())
		{
			_ASSERT(std::find(it->second.begin(), it->second.end(), element) == it->second.end()); //try to add element that is already in the cell, wrong management
		}
#endif
		cellsOfCollisions[cellId].push_back(element); //overwrite if exists
	}

	void addElement(std::shared_ptr<CollisionComponent<T>> element, const CellId newId)
	{
#ifdef _DEBUG
		auto it = cellsOfCollisions.find(newId);
		if (it != cellsOfCollisions.end())
		{
			_ASSERT(std::find(it->second.begin(), it->second.end(), element) == it->second.end()); //try to add element that is already in the cell, wrong management
		}
#endif
		cellsOfCollisions[newId].push_back(element); //overwrite if exists
	}

	[[nodiscard]] std::vector<std::shared_ptr<CollisionComponent<T>>> copyElementsFromCell(CellId id) const noexcept
	{
		if (cellsOfCollisions.find(id) == cellsOfCollisions.end())
			return {};
		return cellsOfCollisions.at(id);
	}

	[[nodiscard]] const std::unordered_map<CellId, std::vector<std::shared_ptr<CollisionComponent<T>>>>& getElements() const noexcept
	{
		return cellsOfCollisions;
	}

	[[nodiscard]] const std::vector<std::shared_ptr<CollisionComponent<T>>>& getElementsFromCell(CellId id) const noexcept
	{
		auto it = cellsOfCollisions.find(id);
		if (it == cellsOfCollisions.end())
		{
			static const std::vector<std::shared_ptr<CollisionComponent<T>>> emptyVector;
			return emptyVector;
		}
		return it->second;
	}

	inline std::vector<CellId> getCellIdsForPosition(FPoint position, float boundRadius) const noexcept
	{
		_ASSERT(boundRadius >= 0.f);

		std::vector<CellId> cellIds;

		CellId centralCell = CollisionSystem::getCellIdForPosition(position);
		auto it = cellsOfCollisions.find(centralCell);
		if (it == cellsOfCollisions.end())
			return {};
		else
			cellIds.push_back(centralCell);

		auto cellSize = CollisionSystem::getCellSize();
		if (boundRadius < cellSize.width && boundRadius < cellSize.height)
		{
			CellId leftUpCell = CollisionSystem::getCellIdForPosition(position - FPoint(boundRadius, boundRadius));
			if (leftUpCell != centralCell)
				cellIds.push_back(leftUpCell);
			CellId rightUpCell = CollisionSystem::getCellIdForPosition(position + FPoint(boundRadius, -boundRadius));
			if (rightUpCell != centralCell)
				cellIds.push_back(rightUpCell);
			CellId leftDownCell = CollisionSystem::getCellIdForPosition(position + FPoint(-boundRadius, boundRadius));
			if (leftDownCell != centralCell)
				cellIds.push_back(leftDownCell);
			CellId rightDownCell = CollisionSystem::getCellIdForPosition(position + FPoint(boundRadius, boundRadius));
			if (rightDownCell != centralCell)
				cellIds.push_back(rightDownCell);
		}
		else
		{
			_ASSERT(false); //not supported yet, would require checking multiple cells around the position
		}
		return cellIds;
	}

	void removeAllCells(std::shared_ptr<CollisionComponent<T>> element)
	{
		_ASSERT(element);
		if (!element)
			return;

		for (const auto& id : element->getOccupiedCells())
			removeElement(element, id);
	}

	void removeElement(std::shared_ptr<CollisionComponent<T>> element, CellId id)
	{
		auto it = cellsOfCollisions.find(id);
		if (it != cellsOfCollisions.end())
		{
			auto vecIt = std::find(it->second.begin(), it->second.end(), element);
			if (vecIt != it->second.end())
				it->second.erase(vecIt);
		}
		else
		{
			_ASSERT(false); //try to remove from non-existing cell
		}
	}

private:
	std::unordered_map< CellId, std::vector<std::shared_ptr<CollisionComponent<T>>>> cellsOfCollisions;
	uint32_t channelId = 0;
};


template<typename T>
class CollisionHandler
{
public:
	CollisionHandler() = default;
	virtual ~CollisionHandler() = default;

	CollisionHandler(const CollisionHandler&) = delete;
	CollisionHandler(const CollisionHandler&&) = delete;
	CollisionHandler operator= (CollisionHandler&) = delete;
	CollisionHandler operator= (CollisionHandler&&) = delete;

	void addCollisionChannel(int newChannelId)
	{
		for (const auto& channel : channels)
		{
			if (channel.getChannelId() == newChannelId)
			{
				_ASSERT(false); //channel with this id already exists
				return;
			}
		}
		this->channels.emplace_back(CollisionChannel<T>{newChannelId});
	}

	void wipeAll()
	{
		channels.clear();
	}

	[[nodiscard]] size_t getChannelSize() noexcept
	{
		return channels.size();
	}

protected:
	std::vector<CollisionChannel<T>> channels;
private:
};

