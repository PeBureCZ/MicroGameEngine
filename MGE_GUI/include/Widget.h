#pragma once
#include <memory>
#include <vector>
#include <variant>

#include "MgeActor.h"
#include "MlWrapper.h"
#include "BasicTypes.h"
#include "BasicShapes.h"
#include "MgeDrawable.h"
#include "GuiDependencies.h"

using WidgetId = uintptr_t;

class Widget : public MgeActor
{
public:
	WidgetId getWidgetId()  const noexcept;
	Widget(const IPoint& newPosition, const ISize& newSize);

	bool removeChildFromWidget(std::variant<WidgetId, std::shared_ptr<MgeActor>> child);

	virtual void setIsVisible(bool visible) noexcept;
	[[nodiscard]] bool getIsVisible() const noexcept;

	void initializeSelf(std::weak_ptr<Widget> self);

	[[nodiscard]] IPoint getRelativePosition() const noexcept;
	[[nodiscard]] IPoint getAbsolutePosition() const noexcept;
	void setRelativePosition(const IPoint& newPosition, bool makeLayout = true)  noexcept;
	void setAbsolutePosition(const IPoint& newPosition, bool makeLayout = true)  noexcept;

	void setSize(const ISize& newSize) noexcept;
	[[nodiscard]] ISize getSize() const noexcept;

	virtual void layout() noexcept;
	virtual ~Widget() = default;
protected:
	std::weak_ptr<Widget> getSelfPtr() const noexcept;

	IPoint lastLayoutAbsolutePosition;

private:
	std::weak_ptr<Widget> selfPtr;
	mgeType::Size<int> size;
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


