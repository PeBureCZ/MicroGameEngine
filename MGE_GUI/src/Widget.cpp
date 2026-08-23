#include "Widget.h"

#include "MlWrapper.h"
#include "MgeDrawable.h"

static MgeDefaultComponent STATIC_PARENT_SYSTEM;

Widget::Widget(const IPoint& newPosition, const ISize& newSize)
	: MgeActor(newPosition.asFloat())
{
	editMgeDefaultComponent().setRelativePosition(newPosition.asFloat());
	editMgeDefaultComponent().setSize(newSize.asFloat());
	lastLayoutAbsolutePosition = newPosition;
}

[[nodiscard]] IPoint Widget::getRelativePosition() const noexcept
{
	return getMgeDefaultComponent().getRelativePosition().asInt();
}

IPoint Widget::getAbsolutePosition() const noexcept
{
	return getMgeDefaultComponent().getAbsolutePosition().asInt();
}

void Widget::setIsVisible(bool visible) noexcept
{
	isVisible = visible;
}

bool Widget::getIsVisible() const noexcept
{
	return isVisible;
}

bool Widget::removeChildFromWidget(std::variant<WidgetId, std::shared_ptr<MgeActor>> child)
{
	if (std::holds_alternative<std::shared_ptr<MgeActor>>(child))
		return editMgeDefaultComponent().removeChild(std::get<std::shared_ptr<MgeActor>>(child));
	else if (std::holds_alternative<WidgetId>(child))
		return editMgeDefaultComponent().removeChild(std::get<WidgetId>(child));
	else
	{
		_ASSERT(false); //unknown alternative
	}
	return false;
}

void Widget::setSize(const ISize& size) noexcept
{
	editMgeDefaultComponent().setSize(size.asFloat());
	layout();
}

std::weak_ptr<Widget> Widget::getSelfPtr() const noexcept
{
	_ASSERT(selfPtr.lock()); //wrong ptr management
	return selfPtr;
}

void Widget::setRelativePosition(const IPoint& newPosition, bool makeLayout) noexcept
{
	editMgeDefaultComponent().setRelativePosition(newPosition.asFloat());
	if (makeLayout)
		layout();
}

void Widget::setAbsolutePosition(const IPoint& newPosition, bool makeLayout) noexcept
{
	editMgeDefaultComponent().setAbsolutePosition(newPosition.asFloat());
	if (makeLayout)
		layout();
}

[[nodiscard]] ISize Widget::getSize() const noexcept
{
	return getMgeDefaultComponent().getSize().asInt();
}

void Widget::layout() noexcept
{
	try
	{
		for (auto& child : getChildren()) //to call layout in children
		{
			if (auto widget = std::dynamic_pointer_cast<Widget>(child))
				widget->layout();
		}
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

WidgetId Widget::getWidgetId() const noexcept
{
	return reinterpret_cast<WidgetId>(this);
}

void Widget::initializeSelf(std::weak_ptr<Widget> self)
{
	selfPtr = self;
}

namespace guiAdd
{
	void addWidgetToGui(std::shared_ptr<Widget> newWidget)
	{
		if (newWidget)
			newWidget->initializeSelf(newWidget);
	}

	void addWidgetToGui(WidgetParent parent, WidgetChild child)
	{
		std::shared_ptr<Widget> validParent;
		if (std::holds_alternative<std::shared_ptr<Widget>>(parent))
			validParent = std::get<std::shared_ptr<Widget>>(parent);
		else if (std::holds_alternative<std::weak_ptr<Widget>>(parent))
			validParent = std::get<std::weak_ptr<Widget>>(parent).lock();

		std::shared_ptr<Widget> validChild;
		if (std::holds_alternative<std::shared_ptr<Widget>>(child))
			validChild = std::get<std::shared_ptr<Widget>>(child);
		else if (std::holds_alternative<std::weak_ptr<Widget>>(child))
			validChild = std::get<std::weak_ptr<Widget>>(child).lock();

		if (!validParent || !validChild || validParent == validChild)
		{
			_ASSERT(false); //cannot be add to self
			return;
		}
		validChild->setParent(validParent);
		validParent->addChild(validChild);
		validChild->initializeSelf(validChild);
	}
}

