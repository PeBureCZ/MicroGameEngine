#pragma once
#include <memory> 
#include <vector>

#include "ScreenEvent.h"
#include "GuiDependencies.h"

#include "BasicShapes.h"
#include "BasicTypes.h"
#include "GraphicDependencies.h"

namespace ML_wrapper
{
	class MlGameWrapper;
}

class MgeActor;

namespace tsmType
{
	template <typename T>
	class Point;
	template <typename T>
	struct Size;
}

class Widget;
class Frame;
class Button;
class BaseScreen;

class MgeGui
{
public:
	MgeGui();
	~MgeGui() = default;

	MgeGui(MgeGui&) = delete;
	MgeGui(MgeGui&&) = delete;
	MgeGui operator=(MgeGui&) = delete;
	MgeGui operator=(MgeGui&&) = delete;

	void TickScreen(BaseScreen& screen, double delta);
	SCREEN_EVENTS& getScreenEventQue();

	void sendLmbEventToGui(const IPoint& clickPos, bool clicked);
	void sendRmbEventToGui(const IPoint& clickPos, bool clicked);
	[[nodiscard]] bool isCursorBlockedByGui() const noexcept;

private:
	std::shared_ptr<ML_wrapper::MlGameWrapper> sharedMlGameWrapper;
	std::vector<std::shared_ptr<Widget>> guiWidgets;
	std::shared_ptr<Widget> actualBlockWidget;

	SCREEN_EVENTS screenEvents; 

	void tickScreenChildren(const BaseScreen& screen);
	void tickWidgets(const std::vector<std::shared_ptr<MgeActor>>& widgets);
	void drawFrameObjects(const Frame& frame);
	[[nodiscard]] bool checkWidgetBlocking(const Frame& frame) const noexcept;
};

