#pragma once
#include "Frame.h"

#include <vector>
#include <memory>

#include "Button.h"

class RollMenu : public Frame
{
public: 
	RollMenu(IPoint newPosition, tsmType::Size<int> newSize = tsmType::Size<int>(100,60), std::string text = "");

	std::shared_ptr<Button> addRollButton(std::string butText, Callback onLMBClickFunc = nullptr, Callback onRMBClickFunc = nullptr);
	std::shared_ptr<Button> getMainButton();

	void setAutoOpen(bool newAutoOpen) noexcept;
	void setAutoClose(bool newAutoClose) noexcept;
	virtual void layout() noexcept override;

protected:
	virtual void onCursorLeaveCall() noexcept override; 
	virtual void onCursorEnterCall() noexcept override;

private:
	std::shared_ptr<Button> mainButton;
	std::shared_ptr<Frame> rollMenuCollisionFrame;

	std::vector<std::weak_ptr<Button>> rollButtons; //weak ptrs to buttons for easier collision management

	std::string mainButtonText = "";
	bool isRolled = false;
	bool autoOpen = false;
	bool autoClose = true;

	void openOrCloseMenu(bool open) noexcept;
	void generateNewRollMenuCollision();
	void onRollMenuLMBClick();
};

