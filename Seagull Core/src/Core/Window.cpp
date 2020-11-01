#include "sgpch.h"
#include "Window.h"

#include "Platform/WindowsWindow.h"

namespace SG
{

	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef SG_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		SG_CORE_ASSERT(false, "Other platform not supported!");
#endif
	}

}