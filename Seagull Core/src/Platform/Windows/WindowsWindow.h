#pragma once

#include "Core/Window.h"

#include <windows.h>

namespace SG
{
#ifdef SG_PLATFORM_WINDOWS

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		~WindowsWindow() = default;

		virtual bool OnCreate() override;
		virtual void OnUpdate() override;

		virtual void SetEventCallbackFn(EventCallbackFn func) override { m_EventCallbackFn = func; }
		virtual EventCallbackFn GetEventCallbackFn() const override { return m_EventCallbackFn; }

		virtual inline uint32_t GetWidth()  const override { return m_WndProps.Width; }
		virtual inline uint32_t GetHeight() const override { return m_WndProps.Height; }

		virtual inline void* GetNativeWindow() const { return m_MainWnd; }
	private:
		bool InitWindowApp(const HINSTANCE& instanceHandle, int show);
	private:
		HWND m_MainWnd;
		WindowProps m_WndProps;
		EventCallbackFn m_EventCallbackFn = nullptr;
	};

#endif
}