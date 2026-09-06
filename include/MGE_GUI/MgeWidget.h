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

enum class WidgetAlignment : int
{
	UpLeft,
	MiddleLeft,
	BottomLeft,
	UpCenter,
	Center,
	BottomCenter,
	UpRight,
	RightCenter,
	BottomRight
};

class MgeWidget : public MgeActor
{
public:
	WidgetId getWidgetId()  const noexcept;
	MgeWidget(const FPoint& newPosition, const ISize& newSize);

	bool removeChildFromWidget(std::variant<WidgetId, std::shared_ptr<MgeActor>> child);

	virtual void setIsVisible(bool visible) noexcept;
	[[nodiscard]] bool getIsVisible() const noexcept;

	void initializeSelf(std::weak_ptr<MgeWidget> self);

	void setAlignment(WidgetAlignment alignment) noexcept;
	WidgetAlignment getAlignment() const noexcept;

	void setSize(const ISize& newSize) noexcept;
	[[nodiscard]] ISize getSize() const noexcept;
	void setMinSize(const ISize& newMinSize) noexcept;
	[[nodiscard]] ISize getMinSize() const noexcept;
	void setMaxSize(const ISize& newMaxSize) noexcept;
	[[nodiscard]] ISize getMaxSize() const noexcept;

	// AutoSizeFactor is a multiplier used in MgeSizer to determine the size during rescaling.
	// It is used to calculate the size of the widget based on its content and the available space in the parent container.
	void setAutoSizeFactor(float factor) noexcept;
	[[nodiscard]] float getAutoSizeFactor() const noexcept;

	[[nodiscard]] bool isInitialized() const noexcept;
	virtual void layout() noexcept;
	virtual void initialize() noexcept; //to be called after adding to parent or to GUI

	void addWidget(std::shared_ptr<MgeWidget> child);

	virtual ~MgeWidget() = default;

protected:
	std::shared_ptr<MgeWidget> getSelfPtr() const noexcept;

	FPoint lastLayoutAbsolutePosition;
	ISize lastLayoutSize;
	FPoint getAlignmentOffset() const noexcept;

private:

	WidgetAlignment m_alignment = WidgetAlignment::UpLeft;

	std::weak_ptr<MgeWidget> m_selfPtr;
	bool m_isVisible = true;
	ISize m_minSize = ISize(1, 1);
	ISize m_maxSize = ISize(7680, 4320);
	float m_autoSizeFactor = 1.0f; //default value, can be changed by user
};




