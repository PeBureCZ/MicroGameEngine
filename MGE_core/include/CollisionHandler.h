#pragma once

#include <vector>

#include "Trigger.h"

template<typename T>
class CollisionChannel
{
public:
	CollisionChannel(int id)
		: channelId(id)
	{}

	[[nodiscard]] int getChannelId() const noexcept
	{
		return channelId;
	}

	void addElement(const T& element)
	{
		elements.push_back(element);
	}

	[[nodiscard]] std::vector<T> copyElements() const noexcept
	{
		return elements;
	}

	void removeElement(const T& element)
	{
		auto it = std::find(elements.begin(), elements.end(), element);
		if (it != elements.end())
		{
			elements.erase(it);
		}
	}

private:
	std::vector<T> elements;
	uint32_t channelId;
};


template<typename T>
class CollisionHandler
{
	public:
		CollisionHandler() = default;
		virtual ~CollisionHandler() = default;

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
			this->channels.emplace_back(newChannelId);
		}

protected:
	std::vector<CollisionChannel<T>> channels;
private:

};

