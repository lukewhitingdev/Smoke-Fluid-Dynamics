#pragma once
#include <iostream>
class InputSystem
{
public:
	struct Mouse
	{
		Mouse() : x(0), y(0) {};
		Mouse(int x, int y) : x(x), y(y) {};
		int x;
		int y;
	};

	static void setMousePosition(int x, int y) {
		setLastMousePosition(mouse.x, mouse.y);

		mouse.x = x; mouse.y = y;
	};
	static Mouse getMousePosition() { return mouse; };

	static void setLastMousePosition(int x, int y) { lastMouse.x = x; lastMouse.y = y; };
	static Mouse getLastMousePosition() { return lastMouse; };

private:
	static Mouse mouse;
	static Mouse lastMouse;
};

