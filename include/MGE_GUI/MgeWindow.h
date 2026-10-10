#pragma once

#include "MgeFrame.h"

#include <memory>
#include <vector>

#include "BasicTypes.h"
#include "MgeImage.h"
#include "GraphicDependencies.h"

constexpr int MIN_WINDOW_HEIGHT_PXLS = 30;
constexpr int MIN_WINDOW_WIDTH_PXLS = 60;
constexpr int DEF_WINDOW_BAR_HEIGHT_PXLS = 30;
constexpr int MAX_WINDOW_BAR_WIDTH_PXLS = 1920 * 4;
constexpr MgeColor DEFAULT_BAR_COLOR = MgeColor(170, 170, 170, 255);


class MgeSizer;
class MgeWindow;
class MgeButton;


namespace mge
{
	using Button = std::shared_ptr<MgeButton>;

	using Window = std::shared_ptr<MgeWindow>;
	Window createEmptyWindow(const FPoint& newPosition = FPoint(), const ISize& size = ISize(1, 1),
		GraphicItemLayer layer = GraphicItemLayer::WINDOW_LAYER, MgeColor color = DEFAULT_FRAME_COLOR);

	Window createCustomWindow(Widget&& content, std::vector<Trigger<int>>&& snapCollision, mge::Button closeButton = nullptr, const FPoint& newPosition = FPoint(),
		GraphicItemLayer layer = GraphicItemLayer::WINDOW_LAYER);
}

class MgeWindow : public MgeFrame
{
public:
	MgeWindow(const FPoint& newPosition, const ISize& size, GraphicItemLayer layer = GraphicItemLayer::WINDOW_LAYER,
		MgeColor color = DEFAULT_FRAME_COLOR);

	void moveWindowContent(std::shared_ptr<MgeSizer>&& sizerWithContent);
	void closeWindow();
	
	void setCloseButton(std::shared_ptr<MgeButton> closeButton) noexcept;
	[[nodiscard]] mge::Button getCloseButton();

	void initialize() noexcept override;

private:

	std::weak_ptr<MgeButton> m_closeButton;
	std::weak_ptr<MgeSizer> m_mainSizer;
};