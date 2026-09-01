#pragma once
#include <memory>
#include <vector>
#include <variant>

#include "Add.h"
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

	virtual void layout() noexcept;
	virtual ~MgeWidget() = default;

protected:
	std::shared_ptr<MgeWidget> getSelfPtr() const noexcept;

	FPoint lastLayoutAbsolutePosition;
	FPoint getAlignmentOffset() const noexcept;

private:

	WidgetAlignment m_alignment = WidgetAlignment::UpLeft;

	std::weak_ptr<MgeWidget> selfPtr;
	mgeType::Size<int> size;
	bool isVisible = true;
};




