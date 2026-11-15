#include "MgeWindow.h"

#include "MgeSizer.h"
#include "MgeButton.h"

//draw group of the front window; every new / raised window takes the next value (GUI runs in the main thread only)
static int64_t g_frontWindowDrawGroup = 0;

MgeWindow::MgeWindow(const FPoint& newPosition, const ISize& size, GraphicItemLayer layer, MgeColor color)
	: MgeFrame(newPosition, size, layer, color)
{
	if (size.height < MIN_WINDOW_HEIGHT_PXLS || size.width < MIN_WINDOW_WIDTH_PXLS)
	{
		ISize newSize((size.height < MIN_WINDOW_HEIGHT_PXLS) ? MIN_WINDOW_HEIGHT_PXLS : size.height,
			(size.width < MIN_WINDOW_WIDTH_PXLS) ? MIN_WINDOW_WIDTH_PXLS : size.width);
		setSize(newSize);
	}

	setBasicCollision();
	setDrawGroup(++g_frontWindowDrawGroup); //new window opens in front; content added later inherits the group
}

//nested windows own their draw group -> they must be raised again to stay above the raised parent window
static void bringNestedWindowsToFront(const std::vector<std::shared_ptr<MgeActor>>& children) noexcept
{
	for (const auto& child : children)
	{
		if (auto window = std::dynamic_pointer_cast<MgeWindow>(child))
			window->bringToFront(); //handles its own nested windows
		else if (child)
			bringNestedWindowsToFront(child->getChildren());
	}
}

void MgeWindow::bringToFront() noexcept
{
	if (isInFront())
		return;

	setDrawGroup(++g_frontWindowDrawGroup);
	bringNestedWindowsToFront(getChildren());
}

bool MgeWindow::isInFront() const noexcept
{
	return getDrawGroup() == g_frontWindowDrawGroup;
}

bool MgeWindow::ownsDrawGroup() const noexcept
{
	return true;
}

void MgeWindow::moveWindowContent(std::shared_ptr<MgeSizer>&& sizerWithContent)
{
	_ASSERT(sizerWithContent.use_count() == 1); //content should be moved and live only here

	if (!sizerWithContent->getParent())
	{
		m_mainSizer = sizerWithContent;
		addWidget(std::move(sizerWithContent));
	}
	else
	{
		_ASSERT(false); //unhandled variation -> added sizer has parent!
	}

}

void MgeWindow::closeWindow()
{
	if (auto closeButton = m_closeButton.lock())
		closeButton->setOnLMBClick(nullptr); //to remove the reference to this window due to ref counting in destructor
	sendEvent(std::move(CloseMgeWindowEvent{ .m_widgetId = getId() }));
}

void MgeWindow::setCloseButton(std::shared_ptr<MgeButton> closeButton) noexcept
{
	auto closeFunction = [this]() { closeWindow(); };
	closeButton->setOnLMBClick(std::move(closeFunction));
	m_closeButton = closeButton;
}

mge::Button MgeWindow::getCloseButton()
{
	return m_closeButton.lock();
}

void MgeWindow::initialize() noexcept
{
	try
	{
		auto windowMainSizer = mge::createSizer(SizerType::VERTICAL);

		auto windowBar = mge::createFrame(FPoint(), ISize(1, 1), GraphicItemLayer::WINDOW_LAYER);
		windowBar->setColor(DEFAULT_BAR_COLOR);
		windowBar->setMinSize(MIN_WINDOW_WIDTH_PXLS, MIN_WINDOW_HEIGHT_PXLS);
		windowBar->setMaxSize(MAX_WINDOW_BAR_WIDTH_PXLS, DEF_WINDOW_BAR_HEIGHT_PXLS);

		auto windowCloseButton = mge::createButton(FPoint(), ISize(MIN_WINDOW_HEIGHT_PXLS, MIN_WINDOW_HEIGHT_PXLS), GraphicItemLayer::WINDOW_LAYER);
		windowCloseButton->setAlignment(GuiAlign::TopRight);
		windowCloseButton->setColor(MgeColor(240, 240, 240));
		windowCloseButton->setHoveredColor(MgeColor(255, 0, 0));
		windowCloseButton->setButtonText("X", 8, GuiAlign::MiddleCenter);
		windowCloseButton->setButtonTextColors(MgeColor(255, 0, 0), MgeColor(0, 0, 255), MgeColor(120,120,120), MgeColor(255, 0, 0), MgeColor(255, 0, 0));
		windowCloseButton->setOnLMBClick([this]() { closeWindow(); });

		windowBar->addWidget(windowCloseButton);

		windowMainSizer->addWidget(windowBar);
		m_mainSizer = windowMainSizer;
		addWidget(std::move(windowMainSizer));

		layout();
	}
	catch (...)
	{
		_ASSERT(false); //unexpected exception
	}
}

namespace mge
{
	Window mge::createEmptyWindow(const FPoint& newPosition, const ISize& size, GraphicItemLayer layer, MgeColor color)
	{
		auto newWindow = std::make_shared<MgeWindow>(newPosition, size, layer, color);
		newWindow->initializeSelf(newWindow);
		newWindow->initialize();
		return newWindow;
	}

	Window createCustomWindow(Widget&& content, std::vector<Trigger<int>>&& snapCollision, mge::Button closeButton, const FPoint& newPosition, GraphicItemLayer layer)
	{
		_ASSERT(content);
		if (!content)
			return std::make_shared<MgeWindow>(newPosition, ISize(1,1), layer, MgeColor(0,0,0,0));

		_ASSERT(content.use_count() == 1); // content should live only there
		auto newWindow = std::make_shared<MgeWindow>(newPosition, content->getSize(), layer, MgeColor(0, 0, 0, 0));
		newWindow->initializeSelf(newWindow);

		if (closeButton)
			newWindow->setCloseButton(closeButton);

		newWindow->addWidget(std::move(content));
		auto& colVec = newWindow->editCollisions();
		colVec = std::move(snapCollision);
		return newWindow;
	}
}

