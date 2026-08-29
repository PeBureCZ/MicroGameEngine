#include "Add.h"

#include "MgeRollMenu.h"
#include "MgeFrame.h"
#include "MgeButton.h"
#include "MgeWidget.h"

namespace mgeGui
{
	void addWidgetToGui(std::shared_ptr<MgeWidget> newWidget)
	{
		if (newWidget)
			newWidget->initializeSelf(newWidget);
	}

	void addWidgetToGui(std::shared_ptr<MgeWidget> parent, std::shared_ptr<MgeWidget> child)
	{
		_ASSERT(parent && child && parent != child);
		if (parent && child && parent != child)
		{
			child->setParent(parent);
			parent->addChild(child);
			child->initializeSelf(child);
		}
	}
}
