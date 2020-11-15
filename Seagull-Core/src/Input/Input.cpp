#include "sgpch.h"
#include "Input.h"

#include "Core/Application.h"
#include "Platform/Windows/WindowsInput.h"


namespace SG
{
#ifdef SG_PLATFORM_WINDOWS
	Input* Input::s_Instance = new WindowsInput();
#endif
}