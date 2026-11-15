#pragma once
#include <memory> 
#include <vector>
#include <limits>

#include "BasicShapes.h"
#include "BasicTypes.h"
#include "GraphicDependencies.h"
#include "MgeWidget.h"

namespace ML_wrapper
{
	class MlWrapper;
}

class MgeActor;
class MgeWindow;

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

	void sendLmbEventToGui(const IPoint& clickPos, bool clicked);
	void sendRmbEventToGui(const IPoint& clickPos, bool clicked);
	[[nodiscard]] bool isCursorBlockedByGui() const noexcept;

	void setFocusedWidget(const mge::Widget& widget);   // nullptr = clear
	void releaseFocus(const mge::Widget& widget);       // clears only if widget is focused
	[[nodiscard]] mge::Widget getFocusedWidget() const noexcept;

private:
	std::shared_ptr<ML_wrapper::MlWrapper> sharedMlWrapper;
	std::vector<mge::Widget> guiWidgets;
	std::weak_ptr<MgeWidget> m_focusedWidget;
	std::weak_ptr<MgeWindow> m_activeWindow; // todo: not implemented yet

	struct ActualBlockWidget
	{
		mge::Widget m_widget;
		int64_t m_zPosition = (std::numeric_limits<int64_t>::min)();
		int64_t m_drawGroup = (std::numeric_limits<int64_t>::min)();
		size_t m_layer = 0;

		void reset()
		{
			m_widget.reset();
			m_zPosition = (std::numeric_limits<int64_t>::min)();
			m_drawGroup = (std::numeric_limits<int64_t>::min)();
			m_layer = 0;
		}
	};

	ActualBlockWidget m_actualBlockWidget;
	bool m_clickEventActive = false;
	FPoint m_lastGuiMovePos; //used for moving MgeWindows

	void tickScreenChildren(const MgeScreen& screen);
	void tickWidgets(const std::vector<std::shared_ptr<MgeActor>>& widgets);
	[[nodiscard]] bool checkWidgetBlocking(const MgeFrame& frame) const noexcept;
	//raises the MgeWindow containing the widget (or the widget itself if it is a window)
	static void bringWindowToFront(const mge::Widget& widget) noexcept;
};

