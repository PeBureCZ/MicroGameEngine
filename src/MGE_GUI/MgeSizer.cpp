#include "MgeSizer.h"

#include <memory>

#include "MgeScreen.h"

MgeSizer::MgeSizer(SizerType type)
	: MgeWidget(FPoint(), ISize(1,1))
{
	m_orientation = type;
	resizeChildren();
}

void MgeSizer::layout() noexcept
{
	if (auto parent = std::dynamic_pointer_cast<MgeWidget>(getParent().value_or(nullptr)))
	{
		if (parent->getSize() != getSize())
			setSize(parent->getSize());
	}
	else
	{
		_ASSERT(false); //Sizer must be a child, in other case, it will not be able to resize its children properly
	}
	resizeChildren();
	MgeWidget::layout();
}

void MgeSizer::resizeChildren() noexcept
{
	MAIN_THREAD_GUARD;

	const bool isVerticalOrientation = (m_orientation == SizerType::VERTICAL);

	const int requiredSizerDimension = static_cast<int>(isVerticalOrientation
		? getSize().height
		: getSize().width);

	FPoint currentRelativePosition{ 0.f, 0.f };

	struct ChildInfo
	{
		std::shared_ptr<MgeWidget> widget;
		ISize size;
		ISize minSize;
		ISize maxSize;
		float autoSizeFactor;
	};

	std::vector<ChildInfo> childrenInfoVec;
	childrenInfoVec.reserve(getChildren().size());

	int actualUsedSize = 0;

	for (auto& child : getChildren())
	{
		_ASSERT(child);

		auto widget = std::dynamic_pointer_cast<MgeWidget>(child);

		if (!widget)
			continue;

		const ISize size = widget->getSize();

		actualUsedSize += isVerticalOrientation
			? size.height
			: size.width;

		childrenInfoVec.emplace_back(ChildInfo
			{
				widget,

				(isVerticalOrientation)
				// maximize second dimension to max sizer size
				? ISize(getSize().width, size.height)
				: ISize(size.width, getSize().height),

				widget->getMinSize(),
				widget->getMaxSize(),
				widget->getAutoSizeFactor()
			});
	}

	if (childrenInfoVec.empty())
		return;

	auto getChildDimension = [&](const ChildInfo& childInfo) -> int
		{
			return isVerticalOrientation
				? childInfo.size.height
				: childInfo.size.width;
		};

	auto getChildMinDimension = [&](const ChildInfo& childInfo) -> int
		{
			return isVerticalOrientation
				? childInfo.minSize.height
				: childInfo.minSize.width;
		};

	auto getChildMaxDimension = [&](const ChildInfo& childInfo) -> int
		{
			return isVerticalOrientation
				? childInfo.maxSize.height
				: childInfo.maxSize.width;
		};

	auto setChildDimension = [&](ChildInfo& childInfo, int dimension)
		{
			if (isVerticalOrientation)
				childInfo.size.height = dimension;
			else
				childInfo.size.width = dimension;
		};

	while (actualUsedSize < requiredSizerDimension)
	{
		const int remainingSpace =
			requiredSizerDimension - actualUsedSize;

		float totalAutoSizeFactor = 0.f;

		for (const auto& childInfo : childrenInfoVec)
		{
			const int currentDimension = getChildDimension(childInfo);
			const int maxDimension = getChildMaxDimension(childInfo);

			if (currentDimension >= maxDimension)
				continue;

			// Factor <= 0 means that this child doesn't participate
			// in automatic growing.
			if (childInfo.autoSizeFactor <= 0.f)
				continue;

			totalAutoSizeFactor += childInfo.autoSizeFactor;
		}

		// No child can grow anymore.
		if (totalAutoSizeFactor <= 0.f)
			break;

		int changedThisPass = 0;

		for (auto& childInfo : childrenInfoVec)
		{
			const int currentDimension = getChildDimension(childInfo);
			const int maxDimension = getChildMaxDimension(childInfo);

			if (currentDimension >= maxDimension)
				continue;

			if (childInfo.autoSizeFactor <= 0.f)
				continue;

			// Calculate this child's proportional part.
			const float proportionalAmount = static_cast<float>(remainingSpace) * (childInfo.autoSizeFactor / totalAutoSizeFactor);

			int amount = static_cast<int>(proportionalAmount);

			// Make sure a child with a positive factor can still
			// receive at least one pixel.
			if (amount <= 0)
				amount = 1;

			// Never exceed max size.
			amount = (std::min)(amount,maxDimension - currentDimension);

			// Never exceed remaining space.
			amount = (std::min)(amount, remainingSpace - changedThisPass);

			if (amount <= 0)
				continue;

			setChildDimension(childInfo,currentDimension + amount);

			actualUsedSize += amount;
			changedThisPass += amount;

			if (actualUsedSize >= requiredSizerDimension)
				break;
		}

		// Safety guard against an infinite loop.
		if (changedThisPass == 0)
			break;
	}

	while (actualUsedSize > requiredSizerDimension)
	{
		const int remainingSpace = actualUsedSize - requiredSizerDimension;
		float totalAutoSizeFactor = 0.f;

		for (const auto& childInfo : childrenInfoVec)
		{
			const int currentDimension = getChildDimension(childInfo);
			const int minDimension = getChildMinDimension(childInfo);

			if (currentDimension <= minDimension)
				continue;

			if (childInfo.autoSizeFactor <= 0.f)
				continue;

			totalAutoSizeFactor += childInfo.autoSizeFactor;
		}

		// No child can shrink anymore.
		if (totalAutoSizeFactor <= 0.f)
			break;

		int changedThisPass = 0;

		for (auto& childInfo : childrenInfoVec)
		{
			const int currentDimension = getChildDimension(childInfo);
			const int minDimension = getChildMinDimension(childInfo);

			if (currentDimension <= minDimension)
				continue;

			if (childInfo.autoSizeFactor <= 0.f)
				continue;

			// Calculate this child's proportional part.
			const float proportionalAmount = static_cast<float>(remainingSpace) * (childInfo.autoSizeFactor / totalAutoSizeFactor);

			int amount = static_cast<int>(proportionalAmount);

			if (amount <= 0)
				amount = 1;

			// Never go below min size.
			amount = (std::min)(amount, currentDimension - minDimension);

			// Never remove more than remaining space.
			amount = (std::min)(amount,remainingSpace - changedThisPass);

			if (amount <= 0)
				continue;

			setChildDimension(childInfo,currentDimension - amount);

			actualUsedSize -= amount;
			changedThisPass += amount;

			if (actualUsedSize <= requiredSizerDimension)
				break;
		}

		// Safety guard against an infinite loop.
		if (changedThisPass == 0)
			break;
	}

	for (auto& childInfo : childrenInfoVec)
	{
		childInfo.widget->setSize(childInfo.size);
		childInfo.widget->setRelativePosition(currentRelativePosition);

		currentRelativePosition += isVerticalOrientation
			? FPoint(0.f,static_cast<float>(childInfo.size.height))
			: FPoint(static_cast<float>(childInfo.size.width),0.f);
	}
}