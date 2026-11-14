#pragma once
#include "MgeFrame.h"

#include <vector>
#include <memory>
#include <string>
#include <string_view>

#include "MgeButton.h"

class MgeRollMenu : public MgeFrame
{
public:
	MgeRollMenu(const FPoint& newPosition, ISize newSize = ISize(100,60), std::string_view text = "");

	// Note: the menu sets a cursor-leave callback (setOnCursorOver) on the returned button for auto close,
	// overriding it disables auto close for that row.
	mge::Button addRollButton(const std::string& butText, VoidCallback onLMBClickFunc = nullptr, VoidCallback onRMBClickFunc = nullptr);
	mge::Button getMainButton();

	void setMainButtonText(std::string_view text);

	void open() noexcept;
	void close() noexcept;
	[[nodiscard]] bool isOpen() const noexcept;

	void setAutoOpen(bool newAutoOpen) noexcept;
	void setAutoClose(bool newAutoClose) noexcept;
	void setIsVisible(bool visible) noexcept override;

protected:
	virtual void onCursorLeaveCall() noexcept override;
	virtual void onCursorEnterCall() noexcept override;

private:
	mge::Button m_mainButton;

	std::vector<std::weak_ptr<MgeButton>> m_rollButtons; //weak ptrs to the rolled buttons (they are also children)

	std::string m_mainButtonText = "";
	bool m_isRolled = false;
	bool m_autoOpen = false;
	bool m_autoClose = true;

	void openOrCloseMenu(bool open) noexcept;
	void onRollMenuLMBClick();
	void onMenuItemCursorLeave() noexcept;
	[[nodiscard]] bool isCursorOverMenu() const noexcept;
};

