#pragma once

#include "../math/vector_type.h"

VKDL_BEGIN

class PlatformWindow;

class Mouse {
public:
	enum Button {
		Left,
		Right,
		Middle,
		XButton1,
		XButton2
	};

	static ivec2 getPosition();
	static ivec2 getPosition(const PlatformWindow& rel_window);
	static void setPosition(const ivec2& pos);
	static void setPosition(const ivec2& pos, const PlatformWindow& rel_window);
	static bool isPressed(Button button);
	static bool isDragging(Button button);
	static bool getDragRect(Button button);
	static bool getDragRect(Button button, const PlatformWindow& rel_window);
};

VKDL_END