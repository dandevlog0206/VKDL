#include "../../include/vkdl/system/mouse.h"

#include "../../include/vkdl/system/platform_window.h"
#include "../../include/vkdl/core/exception.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

VKDL_BEGIN

ivec2 Mouse::getPosition()
{
	POINT point;
	GetCursorPos(&point);
	return { point.x, point.y };
}

ivec2 Mouse::getPosition(const PlatformWindow& rel_window)
{
	HWND hwnd = (HWND)rel_window.getNativeHandle();
	if (hwnd) {
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hwnd, &point);
		return ivec2(point.x, point.y);
	}
	return {};
}

void Mouse::setPosition(const ivec2& pos)
{
	SetCursorPos(pos.x, pos.y);
}

void Mouse::setPosition(const ivec2& pos, const PlatformWindow& rel_window)
{
	HWND hwnd = (HWND)rel_window.getNativeHandle();
	if (hwnd) {
		POINT point{};
		ClientToScreen(hwnd, &point);
		SetCursorPos(point.x, point.y);
	}
}

bool Mouse::isPressed(Button button)
{
	int virtualKey = 0;
	switch (button)
	{
	case Mouse::Left:     virtualKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON; break;
	case Mouse::Right:    virtualKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON; break;
	case Mouse::Middle:   virtualKey = VK_MBUTTON;  break;
	case Mouse::XButton1: virtualKey = VK_XBUTTON1; break;
	case Mouse::XButton2: virtualKey = VK_XBUTTON2; break;
	default:              VKDL_ERROR("unknown mouse button"); break;
	}

	return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

bool Mouse::isDragging(Button button)
{
	return false;
}

bool Mouse::getDragRect(Button button)
{
	return false;
}

bool Mouse::getDragRect(Button button, const PlatformWindow& rel_window)
{
	return false;
}

VKDL_END