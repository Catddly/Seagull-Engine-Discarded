#pragma once

#include "Core.h"
#include <windows.h>

#include "Event/Event.h"
#include "Platform/DirectX/DirectXHelper.h"

namespace SG
{
	struct WindowProps
	{
		uint32_t Width, Height;
		std::wstring Title;
		HINSTANCE WinInstance;
		int Show;
		WindowProps(const HINSTANCE& wndInstace, int show, const std::wstring& t = L"Seagull Engine", uint32_t w = 1920, uint32_t h = 1080)
			:Width(w), Height(h), WinInstance(wndInstace), Show(show), Title(t) {}

		WindowProps(const std::string& t = "Seagull Engine", uint32_t w = 1920, uint32_t h = 1080)
			:Width(w), Height(h), Show(0), WinInstance(nullptr), Title(AnsiToWString(t)) {}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window() = default;
		virtual ~Window() = default;

		virtual bool OnCreate() = 0;
		virtual void OnUpdate() = 0;

		virtual void SetEventCallbackFn(EventCallbackFn func) = 0;
		virtual EventCallbackFn GetEventCallbackFn() const = 0;

		virtual inline uint32_t GetWidth() const = 0;
		virtual inline uint32_t GetHeight() const = 0;

		virtual inline void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}