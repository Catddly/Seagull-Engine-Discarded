#include "sgpch.h"
#include "Input.h"

#include "Core/Application.h"

#ifdef SG_PLATFORM_WINDOWS

#include <WinUser.h>
#include <windef.h>

namespace SG
{

	bool Input::IsKeyPressed(int keycode)
	{
		auto state = GetKeyState(keycode);
		state = state & (1 << 15);
		return bool(state != 0);
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto state = GetKeyState(button);
		state = state & (1 << 15);
		return bool(state != 0);
	}

	std::pair<LONG, LONG> Input::GetMousePos()
	{
		POINT point;
		auto wnd = static_cast<HWND>(Application::Get().GetWindow().GetNativeWindow());
		auto width = Application::Get().GetWindow().GetWidth();
		auto height = Application::Get().GetWindow().GetHeight();
		GetCursorPos(&point);
		ScreenToClient(wnd, &point);
		LONG x = point.x, y = point.y;

		if (x < 0)
			x = 0;
		else if (x > width)
			x = width;

		if (y < 0)
			y = 0;
		else if (y > height)
			y = height;
		return { x, y };
	}

	LONG Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return x;
	}

	LONG Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return y;
	}

}
#endif