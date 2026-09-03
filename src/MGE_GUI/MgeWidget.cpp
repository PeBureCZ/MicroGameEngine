#include "MgeWidget.h"

#include "MlWrapper.h"
#include "MgeDrawable.h"

static MgeDefaultComponent STATIC_PARENT_SYSTEM;

MgeWidget::MgeWidget(const FPoint& newPosition, const ISize& newSize)
	: MgeActor(newPosition)
{
	editMgeDefaultComponent().setRelativePosition(newPosition);
	editMgeDefaultComponent().setSize(newSize.asFloat());
	lastLayoutAbsolutePosition = newPosition;
}

void MgeWidget::setIsVisible(bool visible) noexcept
{
	isVisible = visible;
}

bool MgeWidget::getIsVisible() const noexcept
{
	return isVisible;
}

bool MgeWidget::removeChildFromWidget(std::variant<WidgetId, std::shared_ptr<MgeActor>> child)
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

void MgeWidget::setSize(const ISize& size) noexcept
{
	editMgeDefaultComponent().setSize(size.asFloat());
	for (auto& child : getChildren())
	{
		_ASSERT(child);
		if (!child)
			return;

		if (auto widget = std::dynamic_pointer_cast<MgeWidget>(child))
			widget->setAlignment(widget->getAlignment()); //re-align due to resize
	}
	layout();
}

std::shared_ptr<MgeWidget> MgeWidget::getSelfPtr() const noexcept
{
	_ASSERT(selfPtr.lock()); //wrong ptr management
	return selfPtr.lock();
}

FPoint MgeWidget::getAlignmentOffset() const noexcept
{
	FPoint offset{};
	try
	{
		std::optional<const std::shared_ptr<MgeActor>> parent_opt = getParent();
		if (!parent_opt.has_value() || !parent_opt.value() || !std::dynamic_pointer_cast<MgeWidget>(parent_opt.value()))
			return offset;
	}
	catch (const std::exception& e)
	{
		auto x = e.what();
		_ASSERT(false);
	}
	catch (...)
	{
		_ASSERT(false);
	}

	auto parent_opt = getParent();
	auto parent = std::dynamic_pointer_cast<MgeWidget>(parent_opt.value());

	FSize parentSize = parent->getSize().asFloat();
	auto thisSize = getSize();

	switch (m_alignment)
	{
	case WidgetAlignment::UpLeft: break; 
	case WidgetAlignment::MiddleLeft: offset.x = 0; offset.y = (parentSize.height - thisSize.height) / 2; break; 
	case WidgetAlignment::BottomLeft: offset.x = 0; offset.y = parentSize.height - thisSize.height; break; 
	case WidgetAlignment::UpCenter: offset.x = (parentSize.width - thisSize.width) / 2; offset.y = 0; break;
	case WidgetAlignment::Center: offset.x = (parentSize.width - thisSize.width) / 2; offset.y = (parentSize.height - thisSize.height) / 2; break; 
	case WidgetAlignment::BottomCenter: offset.x = (parentSize.width - thisSize.width) / 2; offset.y = parentSize.height - thisSize.height; break; 
	case WidgetAlignment::UpRight: offset.x = parentSize.width - thisSize.width; offset.y = 0; break; 
	case WidgetAlignment::RightCenter: offset.x = parentSize.width - thisSize.width; offset.y = (parentSize.height - thisSize.height) / 2; break; 
	case WidgetAlignment::BottomRight: offset.x = parentSize.width - thisSize.width; offset.y = parentSize.height - thisSize.height; break; 
	default: _ASSERT(false); break;
	}

	return offset;
}

void MgeWidget::initialize() noexcept
{
	// Could be used for custom initialization in derived classes.
	// The event is published when adding to a parent (or to the GUI for screens),
	// so it doesn't need to be called here.

	// possible to use getSelfPtr() function here
}

void MgeWidget::setAlignment(WidgetAlignment alignment) noexcept
{
	m_alignment = alignment;
	setPositionOffset(getAlignmentOffset());
}

WidgetAlignment MgeWidget::getAlignment() const noexcept
{
	return m_alignment;
}

[[nodiscard]] ISize MgeWidget::getSize() const noexcept
{
	return getMgeDefaultComponent().getSize().asInt();
}

bool MgeWidget::isInitialized() const noexcept
{
	return !selfPtr.expired();
}

void MgeWidget::layout() noexcept
{
	try
	{
		for (auto& child : getChildren()) //to call layout in children
		{
			if (auto widget = std::dynamic_pointer_cast<MgeWidget>(child))
				widget->layout();
		}
	}
	catch (...)
	{
		_ASSERT(false);
	}
}

void MgeWidget::addWidget(std::shared_ptr<MgeWidget> child)
{
	_ASSERT(getSelfPtr() && child && getSelfPtr() != child);
	if (getSelfPtr() && child && getSelfPtr() != child)
	{
		child->setParent(getSelfPtr());
		getSelfPtr()->addChild(child);
		child->initializeSelf(child);

		if (child->getAlignment() != WidgetAlignment::UpLeft)
			child->setAlignment(child->getAlignment()); //re-align offset due to new parent
	}
}

WidgetId MgeWidget::getWidgetId() const noexcept
{
	return reinterpret_cast<WidgetId>(this);
}

void MgeWidget::initializeSelf(std::weak_ptr<MgeWidget> self)
{
	selfPtr = self;
}


