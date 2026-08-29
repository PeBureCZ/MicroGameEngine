#pragma once

#include <variant>
#include <memory>

class MgeWidget;

namespace mgeGui
{
	inline const std::shared_ptr<MgeWidget> NO_PARENT{};

	void addWidgetToGui(std::shared_ptr<MgeWidget> newWidget);
	void addWidgetToGui(std::shared_ptr<MgeWidget> parent, std::shared_ptr<MgeWidget> child);
}

