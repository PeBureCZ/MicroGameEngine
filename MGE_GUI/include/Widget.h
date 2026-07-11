#pragma once
#include <memory>
#include <vector>

#include "MlGameWrapper.h"
#include "BasicTypes.h"
#include "BasicShapes.h"
#include "MlVerticesObject.h"
#include "GuiDependencies.h"

using WidgetId = uintptr_t;

class Widget
{
public:
	WidgetId getWidgetId()  const noexcept;
	Widget(IPoint newPosition, tsmType::Size<int> newSize);


	void setRelativePosition(IPoint newPosition, bool makeLayout = true) noexcept;
	[[nodiscard]] IPoint getRelativePosition() const noexcept;
	[[nodiscard]] IPoint getAbsolutePosition() const noexcept;

	bool removeChildFromWidget(WidgetId childId);
	[[nodiscard]] const std::vector<std::shared_ptr<Widget>>& getChildren() const noexcept;
	[[nodiscard]] std::vector<std::shared_ptr<Widget>>& editChildren() noexcept;
	[[nodiscard]] const std::shared_ptr<Widget> getParent() const noexcept;

	tsmType::Size<int> getSize() const noexcept;

	void setIsVisible(bool visible) noexcept;
	[[nodiscard]] bool getIsVisible() const noexcept;

	void addChild(std::shared_ptr<Widget> child) noexcept;
	void setParent(std::shared_ptr<Widget> newParent = std::shared_ptr<Widget>()) noexcept;
	void initializeSelf(std::weak_ptr<Widget> self);

	virtual void layout() noexcept;
	virtual ~Widget() = default;
protected:
	std::weak_ptr<Widget> getSelfPtr() const noexcept;
	void setSize(tsmType::Size<int> newSize);

	IPoint lastLayoutAbsolutePosition;

private:
	std::weak_ptr<Widget> selfPtr;
	std::weak_ptr<Widget> parent;
	std::vector<std::shared_ptr<Widget>> children;
	IPoint relativePosition;
	tsmType::Size<int> size;
	bool isVisible = true;
};

namespace guiAdd
{
	inline const std::shared_ptr<Widget> NO_PARENT{};
	using WidgetParent = std::variant
		<
			std::weak_ptr<Widget>,
			std::shared_ptr<Widget>
		>;

	using WidgetChild = std::variant
		<
			std::weak_ptr<Widget>,
			std::shared_ptr<Widget>
		>;

	void addWidgetToGui(std::shared_ptr<Widget> newWidget);
	void addWidgetToGui(WidgetParent parent, WidgetChild child);

}


