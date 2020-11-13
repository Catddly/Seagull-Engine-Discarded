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
		auto state = ::GetKeyState(keycode);
		state = state & (1 << 15);
		return bool(state != 0);
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto state = ::GetKeyState(button);
		state = state & (1 << 15);
		return bool(state != 0);
	}

	std::pair<float, float> Input::GetMousePos()
	{
		POINT point = { 0, 0 };
		auto wnd = static_cast<HWND>(Application::Get().GetWindow()->GetNativeWindow());
		::ScreenToClient(wnd, &point);
		return { (float)point.x, (float)point.y };
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return x;
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return y;
	}

}
#endif