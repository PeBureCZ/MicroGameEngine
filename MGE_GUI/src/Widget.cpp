#include "Widget.h"

#include <variant>

#include "MlGameWrapper.h"
#include "MlVerticesObject.h"

Widget::Widget(IPoint newPosition, tsmType::Size<int> newSize)
	: relativePosition(newPosition), size(newSize), lastLayoutAbsolutePosition(newPosition)
{
}

IPoint Widget::getRelativePosition() const noexcept
{
	return relativePosition;
}

IPoint Widget::getAbsolutePosition() const noexcept
{
	if (auto usedParent = getParent())
		return usedParent->getAbsolutePosition() + relativePosition;
	return relativePosition;
}

void Widget::setIsVisible(bool visible) noexcept
{
	isVisible = visible;
}

bool Widget::getIsVisible() const noexcept
{
	return isVisible;
}

void Widget::addChild(std::shared_ptr<Widget> child) noexcept
{
	children.push_back(std::move(child));
}

bool Widget::removeChildFromWidget(WidgetId childId) noexcept
{
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		if (auto& child = *it)
		{
			if (child->getWidgetId() == childId)
			{
				child->setParent();
				children.erase(it);
				return true;
			}
		}
	}
	_ASSERT(false); //try to remove a non-existent child
	return false;
}

const std::vector<std::shared_ptr<Widget>>& Widget::getChildren() const noexcept
{
	return children;
}

std::vector<std::shared_ptr<Widget>>& Widget::editChildren() noexcept
{
	return children;
}

void Widget::setParent(std::shared_ptr<Widget> newParent) noexcept
{
	parent = std::move(newParent);
}

const std::shared_ptr<Widget> Widget::getParent() const noexcept
{
	return parent.lock();
}

void Widget::setSize(tsmType::Size<int> newSize)
{
	size = newSize;
	layout();
}

std::weak_ptr<Widget> Widget::getSelfPtr() const noexcept
{
	_ASSERT(selfPtr.lock()); //wrong ptr management
	return selfPtr;
}

void Widget::setRelativePosition(IPoint newPosition, bool makeLayout)  noexcept
{
	relativePosition = newPosition;
	if (makeLayout)
		layout();
}

tsmType::Size<int> Widget::getSize() const noexcept
{
	return size;
}

void Widget::layout() noexcept
{
	for (auto& child : getChildren()) //to call layout in children
		child->layout();
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

