#include "MgeWindow.h"

#include "MgeSizer.h"
#include "MgeButton.h"

MgeWindow::MgeWindow(const FPoint& newPosition, const ISize& size, GraphicItemLayer layer, mgeType::Color_RGBA color)
	: MgeFrame(newPosition, size, layer, color)
{
	if (size.height < MIN_WINDOW_HEIGHT_PXLS || size.width < MIN_WINDOW_WIDTH_PXLS)
	{
		ISize newSize((size.height < MIN_WINDOW_HEIGHT_PXLS) ? MIN_WINDOW_HEIGHT_PXLS : size.height,
			(size.width < MIN_WINDOW_WIDTH_PXLS) ? MIN_WINDOW_WIDTH_PXLS : size.width);
		setSize(newSize);
	}

	editCollision().push_back(Trigger<int>(true, mgeShape::Rectangle<int>(getAbsolutePosition().asInt(), getSize())));
	_ASSERT(editCollision().size() == 1);
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
	//destroy();
	sendEvent(std::move(CloseMgeWindowEvent{ .m_widgetId = getId() }));
}

void MgeWindow::initialize() noexcept
{
	auto windowMainSizer = mge::createSizer(SizerType::VERTICAL);

	auto windowBar = mge::createFrame(FPoint(), ISize(1, 1), GraphicItemLayer::WINDOW_LAYER);
	windowBar->setColor(DEFAULT_BAR_COLOR);
	windowBar->setMinSize(MIN_WINDOW_WIDTH_PXLS, MIN_WINDOW_HEIGHT_PXLS);
	windowBar->setMaxSize(MAX_WINDOW_BAR_WIDTH_PXLS, DEF_WINDOW_BAR_HEIGHT_PXLS);

	auto windowCloseButton = mge::createButton(FPoint(), ISize(MIN_WINDOW_HEIGHT_PXLS, MIN_WINDOW_HEIGHT_PXLS), GraphicItemLayer::WINDOW_LAYER);
	windowCloseButton->setAlignment(GuiAlign::TopRight);
	windowCloseButton->setDefaultButtonColor(mgeType::Color_RGBA(240, 240, 240));
	windowCloseButton->setMouseOverButtonColor(mgeType::Color_RGBA(255, 0, 0));
	windowCloseButton->addTextToFrame("X", 8, GuiAlign::MiddleCenter, mgeType::Color_RGBA(255, 0, 0));
	windowCloseButton->setButtonTextColors(mgeType::Color_RGBA(255, 0, 0), mgeType::Color_RGBA(0,0,255));
	windowCloseButton->setOnLMBClick([this]() { closeWindow(); });

	windowBar->addWidget(windowCloseButton);

	windowMainSizer->addWidget(windowBar);
	m_mainSizer = windowMainSizer;
	addWidget(std::move(windowMainSizer));

	layout();
}

namespace mge
{
	Window mge::createEmptyWindow(const FPoint& newPosition, const ISize& size, GraphicItemLayer layer, mgeType::Color_RGBA color)
	{
		auto newWindow = std::make_shared<MgeWindow>(newPosition, size, layer, color);
		newWindow->initializeSelf(newWindow);
		newWindow->initialize();
		return newWindow;
	}

	Window createCustomWindow(Widget&& content, std::vector<Trigger<int>>&& snapCollision, const FPoint& newPosition, GraphicItemLayer layer)
	{
		_ASSERT(content);
		if (!content)
			return std::make_shared<MgeWindow>(newPosition, ISize(1,1), layer, mgeType::Color_RGBA(0,0,0,0));

		_ASSERT(content.use_count() == 1); // content should live only there
		auto newWindow = std::make_shared<MgeWindow>(newPosition, content->getSize(), layer, mgeType::Color_RGBA(0, 0, 0, 0));
		newWindow->initializeSelf(newWindow);
		newWindow->addWidget(std::move(content));
		auto& colVec = newWindow->editCollision();
		colVec = std::move(snapCollision);
		return newWindow;
	}
}

