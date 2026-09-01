#pragma once
#include <variant>
#include <vector>
#include <deque>
#include <memory>
#include <utility>
#include <functional>

#include "MgeWidget.h"
#include "Trigger.h"
#include "MgeImage.h"
#include "MgeText.h"
#include "MgeDrawable.h"
#include "GuiDependencies.h"

constexpr mgeType::Color_RGBA DEFAULT_TEXT_COLOR = mgeType::Color_RGBA(0, 0, 0, 255);
constexpr mgeType::Color_RGBA MOUSE_OVER_TEXT_COLOR = mgeType::Color_RGBA(0, 0, 0, 255);

class MgeButton;
using Callback_deprecated = std::function<void()>;

template<typename T>
class Trigger;

using UNDEFINED_FRAME_OBJECT = bool;
using FRAME_OBJECT = std::variant<UNDEFINED_FRAME_OBJECT, MgeImage, MgeDrawable>;

class MgeFrame : public MgeWidget
{
public:
	MgeFrame(const FPoint& newPosition, const ISize& newSize);
	MgeFrame(const FPoint& newPosition, TextureId textureId);

	void setImage(TextureId textureId);
	void setVertices(MgeDrawable&& newVertices) noexcept;
	[[nodiscard]] std::vector<Trigger<int>>& editCollision() noexcept;
	[[nodiscard]] const std::vector<Trigger<int>>& getCollision() const noexcept;
	void addCollision(Trigger<int> addedCollision);

	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	void setColor(const mgeType::Color_RGBA& newColor);
	[[nodiscard]] const FRAME_OBJECT& getFrameObject() const noexcept;
	void addTextToFrame(const std::string& butText, unsigned int characterSize_pxls = 30, GuiAlign align = GuiAlign::MiddleCenter,
		const mgeType::Color_RGBA& col = DEFAULT_TEXT_COLOR);
	const std::deque<std::pair<GuiAlign, MgeText>>& getTextsFromFrame() const noexcept;

	void setIsVisible(bool visible) noexcept override;

	void setRelativeRotation(float newRotation);
	float getRelativeRotation(); 
	void setOrigin(IPoint newOrigin);

	//to define function which will be call when mouse is over the widget
	void setOnCursorOver(Callback_deprecated cursorEnterFunction, Callback_deprecated cursorLeaveFunction = nullptr);

	//function is called automatically from GUI
	void setUnderMouseCursor(bool isUnderMouse);

	void layout() noexcept override;

	[[nodiscard]] bool isUnderCursor() const noexcept;

protected:
	std::deque<std::pair<GuiAlign, MgeText>> frameTexts;

	//to call specific defined function (eg. from another widget)
	virtual void onCursorEnterCall() noexcept;
	//to call specific defined function (eg. from another widget)
	virtual void onCursorLeaveCall() noexcept;
	bool isUnderMouseCursor = false;

private:
	std::vector<Trigger<int>> collisions;
	FRAME_OBJECT frameObject = false;

	std::function<void()> onCursorEnter = nullptr;
	std::function<void()> onCursorLeave = nullptr;

	IPoint getAlignedPosition(GuiAlign align, mgeType::Size<int> objectSize);
};

