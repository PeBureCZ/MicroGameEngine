#pragma once
#include <variant>
#include <vector>
#include <deque>
#include <memory>
#include <utility>
#include <functional>

#include "Widget.h"
#include "Trigger.h"
#include "MlImage.h"
#include "MlText.h"
#include "GuiDependencies.h"

class Button;
using Callback = std::function<void()>;

template<typename T>
class Trigger;

using UNDEFINED_FRAME_OBJECT = bool;
using FRAME_OBJECT = std::variant<UNDEFINED_FRAME_OBJECT, DrawableObject<float>, std::shared_ptr<MlImage>>;

class Frame : public Widget
{
public:
	Frame(IPoint newPosition, mgeType::Size<int> newSize);
	Frame(IPoint newPosition, TextureId textureId);

	void setImage(TextureId textureId);
	void setVerticesId(VerticesId newId) noexcept;
	[[nodiscard]] std::vector<Trigger<int>>& editCollision() noexcept;
	[[nodiscard]] const std::vector<Trigger<int>>& getCollision() const noexcept;
	void addCollision(Trigger<int> addedCollision);

	void setColor(mgeType::Color_RGBA newColor);
	[[nodiscard]] const FRAME_OBJECT& getFrameObject() const noexcept;
	[[nodiscard]] VerticesId getVerticesId() const noexcept;
	void addTextToFrame(MlText newText, GuiAlign align = GuiAlign::MiddleCenter);
	const std::deque<std::pair<GuiAlign, MlText>>& getTextsFromFrame() const noexcept;

	void setRelativeRotation(float newRotation);
	float getRelativeRotation();
	void setOrigin(IPoint newOrigin);

	//to define function which will be call when mouse is over the widget
	void setOnCursorOver(Callback cursorEnterFunction, Callback cursorLeaveFunction = nullptr);

	//function is called automatically from GUI
	void setUnderMouseCursor(bool isUnderMouse);

	void layout() noexcept override;

	[[nodiscard]] bool isUnderCursor() const noexcept;

protected:
	std::deque<std::pair<GuiAlign, MlText>> frameTexts;

	//to call specific defined function (eg. from another widget)
	virtual void onCursorEnterCall() noexcept;
	//to call specific defined function (eg. from another widget)
	virtual void onCursorLeaveCall() noexcept;
	bool isUnderMouseCursor = false;

private:
	std::vector<Trigger<int>> collisions;
	VerticesId mlVerticesId = 0;
	FRAME_OBJECT frameObject = false;

	std::function<void()> onCursorEnter = nullptr;
	std::function<void()> onCursorLeave = nullptr;

	IPoint getAlignedPosition(GuiAlign align, mgeType::Size<int> objectSize);
};

