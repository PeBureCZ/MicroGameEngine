#include "MgeWidget.h"

#include "MlWrapper.h"
#include "MgeDrawable.h"

static MgeDefaultComponent STATIC_PARENT_SYSTEM;

MgeWidget::MgeWidget(const FPoint& newPosition, const ISize& newSize)
	: MgeActor(newPosition)
{
	_ASSERT(newSize.width > 0 && newSize.height > 0);
	editMgeDefaultComponent().setRelativePosition(newPosition);
	editMgeDefaultComponent().setSize(newSize.asFloat());
	lastLayoutSize = newSize;
	lastLayoutAbsolutePosition = newPosition;
}

void MgeWidget::setIsVisible(bool visible) noexcept
{
	m_isVisible = visible;
}

bool MgeWidget::getIsVisible() const noexcept
{
	return m_isVisible;
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
#ifdef _DEBUG
	[[maybe_unused]] auto id = getId();
	if (id > 0)
		id = id;
#endif
	ISize newSize = size;

	_ASSERT(m_minSize.width <= m_maxSize.width);
	_ASSERT(m_minSize.height <= m_maxSize.height);
	if (m_maxSize.width < m_minSize.width)
		m_maxSize.width = m_minSize.width;
	if (m_maxSize.height < m_minSize.height)
		m_maxSize.height = m_minSize.height;

	newSize.width = std::clamp(newSize.width, m_minSize.width, m_maxSize.width);
	newSize.height = std::clamp(newSize.height, m_minSize.height, m_maxSize.height);

	editMgeDefaultComponent().setSize(newSize.asFloat());
	for (auto& child : getChildren())
	{
		_ASSERT(child);
		if (!child)
			return;

		if (auto widget = std::dynamic_pointer_cast<MgeWidget>(child))
			widget->setAlignment(widget->getAlignment()); //re-align due to resize
	}
}

mge::Widget MgeWidget::getSelfPtr() const noexcept
{
	_ASSERT(m_selfPtr.lock()); //wrong ptr management
	return m_selfPtr.lock();
}

FPoint MgeWidget::getAlignmentOffset() const noexcept
{
	FPoint offset{};
	mge::Widget widget;
	try
	{
		widget = std::dynamic_pointer_cast<MgeWidget>(getParent());
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

	if (!widget)
		return offset;

	FSize parentSize = widget->getSize().asFloat();
	auto thisSize = getSize();

	switch (m_alignment)
	{
	case GuiAlign::TopLeft: break;
	case GuiAlign::MiddleLeft: offset.x = 0; offset.y = (parentSize.height - thisSize.height) / 2; break;
	case GuiAlign::BottomLeft: offset.x = 0; offset.y = parentSize.height - thisSize.height; break;
	case GuiAlign::TopCenter: offset.x = (parentSize.width - thisSize.width) / 2; offset.y = 0; break;
	case GuiAlign::MiddleCenter: offset.x = (parentSize.width - thisSize.width) / 2; offset.y = (parentSize.height - thisSize.height) / 2; break;
	case GuiAlign::BottomCenter: offset.x = (parentSize.width - thisSize.width) / 2; offset.y = parentSize.height - thisSize.height; break;
	case GuiAlign::TopRight: offset.x = parentSize.width - thisSize.width; offset.y = 0; break;
	case GuiAlign::MiddleRight: offset.x = parentSize.width - thisSize.width; offset.y = (parentSize.height - thisSize.height) / 2; break;
	case GuiAlign::BottomRight: offset.x = parentSize.width - thisSize.width; offset.y = parentSize.height - thisSize.height; break;
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

void MgeWidget::setAlignment(GuiAlign alignment) noexcept
{
	m_alignment = alignment;
	setPositionOffset(getAlignmentOffset());
}

GuiAlign MgeWidget::getAlignment() const noexcept
{
	return m_alignment;
}

[[nodiscard]] ISize MgeWidget::getSize() const noexcept
{
	return getMgeDefaultComponent().getSize().asInt();
}

void MgeWidget::setMinSize(const ISize& newMinSize) noexcept
{
	m_minSize = newMinSize;
}

ISize MgeWidget::getMinSize() const noexcept
{
	return m_minSize;
}

void MgeWidget::setMaxSize(const ISize& newMaxSize) noexcept
{
	m_maxSize = newMaxSize;
}

ISize MgeWidget::getMaxSize() const noexcept
{
	return m_maxSize;
}

void MgeWidget::setAutoSizeFactor(float factor) noexcept
{
	m_autoSizeFactor = factor;
}

float MgeWidget::getAutoSizeFactor() const noexcept
{
	return m_autoSizeFactor;
}

bool MgeWidget::isInitialized() const noexcept
{
	return !m_selfPtr.expired();
}

void MgeWidget::layout() noexcept
{
#ifdef _DEBUG
	[[maybe_unused]] auto id = getId();
	if (id > 0)
		id = id;
#endif
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
	lastLayoutSize = getSize();
	lastLayoutAbsolutePosition = getAbsolutePosition();
}

void MgeWidget::addWidget(mge::Widget child)
{
	_ASSERT(getSelfPtr() && child && getSelfPtr() != child);
	if (getSelfPtr() && child && getSelfPtr() != child)
	{
		child->setParent(getSelfPtr());
		addChild(child);
		child->initializeSelf(child);

		if (child->getAlignment() != GuiAlign::TopLeft)
			child->setAlignment(child->getAlignment()); //re-align offset due to new parent
	}
}

void MgeWidget::closeWidget()
{
	for (auto& child : editChildren())
	{
		_ASSERT(child);
		if (auto widget = std::dynamic_pointer_cast<MgeWidget>(child))
			widget->closeWidget();
	}

	if (auto parent = getParent())
	{
		[[maybe_unused]] bool removed = parent->removeChild(getId()); //remove self in parent vector (owner)
	}
}

WidgetId MgeWidget::getWidgetId() const noexcept
{
	return reinterpret_cast<WidgetId>(this);
}

void MgeWidget::initializeSelf(std::weak_ptr<MgeWidget> self)
{
	m_selfPtr = self;
}

namespace mge
{
	Widget mge::createWidget(const FPoint& position, const ISize& size)
	{
		return std::make_shared<MgeWidget>(position, size);
	}
}
