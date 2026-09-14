#pragma once
#include "MgeFrame.h"

#include <vector>
#include <memory>
#include <string_view>

#include "MgeButton.h"

class MgeRollMenu : public MgeFrame
{
public: 
	MgeRollMenu(const FPoint& newPosition, ISize newSize = ISize(100,60), std::string_view text = "");

	mge::Button addRollButton(const std::string& butText, Callback_deprecated onLMBClickFunc = nullptr, Callback_deprecated onRMBClickFunc = nullptr);
	mge::Button getMainButton();

	void setAutoOpen(bool newAutoOpen) noexcept;
	void setAutoClose(bool newAutoClose) noexcept;
	virtual void layout() noexcept override;
	void setIsVisible(bool visible) noexcept override;

protected:
	virtual void onCursorLeaveCall() noexcept override; 
	virtual void onCursorEnterCall() noexcept override;

private:
	mge::Button mainButton;
	std::shared_ptr<MgeFrame> rollMenuCollisionFrame;

	std::vector<std::weak_ptr<MgeButton>> rollButtons; //weak ptrs to buttons for easier collision management

	std::string mainButtonText = "";
	bool isRolled = false;
	bool autoOpen = false;
	bool autoClose = true;

	void openOrCloseMenu(bool open) noexcept;
	void generateNewRollMenuCollision();
	void onRollMenuLMBClick();
};

