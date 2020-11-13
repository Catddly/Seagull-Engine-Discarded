#include "sgpch.h"
#include "WindowsWindow.h"

#include "Event/MouseEvent.h"
#include "Event/KeyboardEvent.h"
#include "Event/ApplicationEvent.h"

#include "Core/Application.h"
#include "ImGui/ImGuiLayer.h"

#include <windowsx.h>

namespace SG
{
	LRESULT CALLBACK WndProc(HWND hWnd /*window*/, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		auto window = static_cast<WindowsWindow*>(Application::Get().GetWindow());
		LRESULT value = window->WindowProcess(hWnd, msg, wParam, lParam);
		if (value == 0)
			return 0;
		else
			return value;
	}

	// WPARAM 和 LPARAM 是消息响应机制
	LRESULT CALLBACK WindowsWindow::WindowProcess(HWND hWnd /*window*/, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// TODO: change key code to SG key code
		switch (msg)
		{
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
			case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
			{
				int button = 0;
				if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = VK_LBUTTON;
				if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = VK_RBUTTON;
				if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = VK_MBUTTON;
				if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) 
					button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
				MouseButtonPressedEvent e(button, 0); m_EventCallbackFn(e);
				return 0;
			}
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
			{
				int button = 0;
				if (msg == WM_LBUTTONUP) button = VK_LBUTTON;
				if (msg == WM_RBUTTONUP) button = VK_RBUTTON;
				if (msg == WM_MBUTTONUP) button = VK_MBUTTON;
				if (msg == WM_XBUTTONUP) 
					button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
				MouseButtonReleasedEvent e(button); m_EventCallbackFn(e);
				return 0;
			}
			case WM_MOUSEWHEEL:
			{
				float OffsetV = (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
				MouseScrolledEvent e(0, OffsetV); m_EventCallbackFn(e);
				return 0;
			}
			case WM_MOUSEHWHEEL:
			{
				float OffsetH = (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
				MouseScrolledEvent e(OffsetH, 0); m_EventCallbackFn(e);
				return 0;
			}
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				if (wParam < 256) {
					KeyPressedEvent e(wParam, LOWORD(lParam));
					m_EventCallbackFn(e);
				}
				return 0;
			}
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				if (wParam < 256) {
					KeyReleasedEvent e(wParam);
					m_EventCallbackFn(e);
				}
				return 0;
			}
			case WM_CHAR:
			{
				if (wParam > 0 && wParam < 0x10000)
				{
					KeyTypedEvent e((unsigned short)wParam);
					m_EventCallbackFn(e);
				}
				return 0;
			}
			case WM_SETCURSOR:
			{
				MouseFocusWindowChangedEvent e((void*)lParam, LOWORD(wParam), HIWORD(wParam));
				m_EventCallbackFn(e);
				return 0;
			}
			case WM_DEVICECHANGE:
			{
				DeviceChangedEvent e((UINT)wParam, (void*)lParam);
				m_EventCallbackFn(e);
				return 0;
			}
			case WM_MOUSEMOVE:
			{
				// low 16 bits for x, high 16 bits for y
				int xPos = GET_X_LPARAM(lParam);
				int yPos = GET_Y_LPARAM(lParam);
				MouseMovedEvent e(xPos, yPos); m_EventCallbackFn(e);
				return 0;
			}
			case WM_SIZE:
			{
				auto width = (uint32_t)LOWORD(lParam);
				auto height = (uint32_t)HIWORD(lParam);
				m_WndProps.Width = width; m_WndProps.Height = height;
				WindowResizeEvent e(width, height); 
				m_EventCallbackFn(e);
				return 0;
			}
			case WM_SYSCOMMAND:
			{
				if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
					return 0;
			}
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
		: m_MainWnd(0), m_WndProps(props)
	{
	}

	WindowsWindow::~WindowsWindow()
	{
		::UnregisterClass(L"MainWndClass", m_WndProps.WinInstance);
	}

	bool WindowsWindow::OnCreate()
	{
		return InitWindowApp(m_WndProps.WinInstance, m_WndProps.Show);
	}

	void WindowsWindow::OnUpdate()
	{
	}

	bool WindowsWindow::InitWindowApp(const HINSTANCE& instanceHandle, int show)
	{
		// fill a window class to create a window
		WNDCLASSEX myMainWnd;

		myMainWnd.cbSize = sizeof(WNDCLASSEX);
		myMainWnd.style = CS_HREDRAW | CS_VREDRAW; // redraw the window if width or height changed
		myMainWnd.lpfnWndProc = WndProc;
		myMainWnd.cbClsExtra = 0;
		myMainWnd.cbWndExtra = 0;
		myMainWnd.hInstance = instanceHandle;
		myMainWnd.hIcon = LoadIcon(0, IDI_APPLICATION);
		myMainWnd.hCursor = LoadCursor(0, IDC_ARROW);
		myMainWnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		myMainWnd.lpszMenuName = 0;
		myMainWnd.lpszClassName = L"MainWndClass";
		myMainWnd.hIconSm = nullptr;

		// register an instance for myMainWnd
		if (!RegisterClassEx(&myMainWnd))
		{
			MessageBox(0, L"Register class FAILED", L"Error", 0);
			return false;
		}

		// Create a window
		m_MainWnd = CreateWindow(
			L"MainWndClass",
			m_WndProps.Title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, // x
			CW_USEDEFAULT, // y
			m_WndProps.Width, // width
			m_WndProps.Height, // height
			0, // parent window
			0, // menu handle
			instanceHandle, // application instance handle
			0);

		// fail to create window
		if (m_MainWnd == 0)
		{
			MessageBox(0, L"Create window FAILED", L"Error", 0);
			return false;
		}

		ShowWindow(m_MainWnd, show);
		UpdateWindow(m_MainWnd);

		return true;
	}

}