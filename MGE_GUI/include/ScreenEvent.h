#pragma once

#include <memory>
#include <deque>
#include <variant>
#include <string>
#include <vector>

using EventType = int;
using EVENT_DATA = std::variant
	<
		bool,
		int32_t,
		std::string,
		std::vector<std::string>,
		std::vector<int32_t>,
		uintptr_t
	>;

class ScreenEvent
{
	public:

	ScreenEvent(EventType typeOfEvent)
		: eventType(typeOfEvent)
	{}
	EventType eventType = -1;
	EVENT_DATA eventData;
};

using EVENT_QUE = std::shared_ptr<std::deque<ScreenEvent>>;

using SCREEN_EVENTS = EVENT_QUE;
