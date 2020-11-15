#include "sgpch.h"
#include "WindowsInput.h"

#include "Core/Application.h"

#ifdef SG_PLATFORM_WINDOWS

#include <WinUser.h>
#include <windef.h>

namespace SG
{

	bool WindowsInput::IsKeyPressedImpl(int keycode) const
	{
		auto state = ::GetKeyState(keycode);
		state = state & (1 << 15);
		return bool(state != 0);
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button) const
	{
		auto state = ::GetKeyState(button);
		state = state & (1 << 15);
		return bool(state != 0);
	}

	std::pair<float, float> WindowsInput::GetMousePosImpl() const
	{
		POINT point = { 0, 0 };
		auto wnd = static_cast<HWND>(Application::Get().GetWindow()->GetNativeWindow());
		::GetCursorPos(&point);
		::ScreenToClient(wnd, &point);
		return { (float)point.x, (float)point.y };
	}

	float WindowsInput::GetMouseXImpl() const
	{
		auto [x, y] = GetMousePos();
		return x;
	}

	float WindowsInput::GetMouseYImpl() const
	{
		auto [x, y] = GetMousePos();
		return y;
	}

}
#endif