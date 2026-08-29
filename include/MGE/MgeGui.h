#pragma once
#include <memory> 
#include <vector>

#include "GuiDependencies.h"

#include "BasicShapes.h"
#include "BasicTypes.h"
#include "GraphicDependencies.h"

namespace ML_wrapper
{
	class MlWrapper;
}

class MgeActor;

namespace tsmType
{
	template <typename T>
	class Point;
	template <typename T>
	struct Size;
}

class MgeWidget;
class MgeFrame;
class MgeButton;
class MgeScreen;

class MgeGui
{
public:
	MgeGui();
	~MgeGui() = default;

	MgeGui(MgeGui&) = delete;
	MgeGui(MgeGui&&) = delete;
	MgeGui operator=(MgeGui&) = delete;
	MgeGui operator=(MgeGui&&) = delete;

	void TickScreen(MgeScreen& screen, double delta);
	//SCREEN_EVENTS& getScreenEventQue();

	void sendLmbEventToGui(const IPoint& clickPos, bool clicked);
	void sendRmbEventToGui(const IPoint& clickPos, bool clicked);
	[[nodiscard]] bool isCursorBlockedByGui() const noexcept;

private:
	std::shared_ptr<ML_wrapper::MlWrapper> sharedMlWrapper;
	std::vector<std::shared_ptr<MgeWidget>> guiWidgets;
	std::shared_ptr<MgeWidget> actualBlockWidget;

	//SCREEN_EVENTS screenEvents; 

	void tickScreenChildren(const MgeScreen& screen);
	void tickWidgets(const std::vector<std::shared_ptr<MgeActor>>& widgets);
	[[nodiscard]] bool checkWidgetBlocking(const MgeFrame& frame) const noexcept;
};

