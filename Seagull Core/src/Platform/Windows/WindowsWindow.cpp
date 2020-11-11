#include "sgpch.h"
#include "WindowsWindow.h"

#include "Event/MouseEvent.h"
#include "Event/KeyboardEvent.h"
#include "Event/ApplicationEvent.h"

#include "Core/Application.h"

#include <windowsx.h>

namespace SG
{
	// WPARAM 和 LPARAM 是消息响应机制
	LRESULT CALLBACK WindowProcess(HWND hWnd /*window*/, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// TODO: change key code to SG key code
		auto fn = Application::Get().GetWindow().GetEventCallbackFn();
		switch (msg)
		{
			case WM_LBUTTONDOWN:
			{
				MouseButtonPressedEvent e(VK_LBUTTON, 0); fn(e);
				return 0;
			}
			case WM_RBUTTONDOWN:
			{
				MouseButtonPressedEvent e(VK_RBUTTON, 0); fn(e);
				return 0;
			}
			case WM_MBUTTONDOWN:
			{
				MouseButtonPressedEvent e(VK_MBUTTON, 0); fn(e);
				return 0;
			}
			case WM_KEYDOWN:
			{
				if (wParam == VK_ESCAPE) { WindowCloseEvent e(&Application::Get().GetWindow()); fn(e); }
				if (wParam == 'W') { KeyPressedEvent e('W', LOWORD(lParam)); fn(e); }
				if (wParam == 'A') { KeyPressedEvent e('A', LOWORD(lParam)); fn(e); }
				if (wParam == 'S') { KeyPressedEvent e('S', LOWORD(lParam)); fn(e); }
				if (wParam == 'D') { KeyPressedEvent e('D', LOWORD(lParam)); fn(e); }
				if (wParam == 'M') { KeyPressedEvent e('M', LOWORD(lParam)); fn(e); }
				return 0;
			}
			case WM_MOUSEMOVE:
			{
				// low 16 bits for x, high 16 bits for y
				int xPos = GET_X_LPARAM(lParam);
				int yPos = GET_Y_LPARAM(lParam);
				MouseMovedEvent e(xPos, yPos); fn(e);
				return 0;
			}
			case WM_SIZE:
			{
				auto width = LOWORD(lParam);
				auto height = HIWORD(lParam);
				WindowResizeEvent e(width, height); 
				fn(e);
				return 0;
			}
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
		: m_MainWnd(0), m_WndProps(props)
	{
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
		WNDCLASS myMainWnd;

		myMainWnd.style = CS_HREDRAW | CS_VREDRAW; // redraw the window if width or height changed
		myMainWnd.lpfnWndProc = WindowProcess;
		myMainWnd.cbClsExtra = 0;
		myMainWnd.cbWndExtra = 0;
		myMainWnd.hInstance = instanceHandle;
		myMainWnd.hIcon = LoadIcon(0, IDI_APPLICATION);
		myMainWnd.hCursor = LoadCursor(0, IDC_ARROW);
		myMainWnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		myMainWnd.lpszMenuName = 0;
		myMainWnd.lpszClassName = L"BasicWndClass";

		// register an instance for myMainWnd
		if (!RegisterClass(&myMainWnd))
		{
			MessageBox(0, L"Register class FAILED", L"Error", 0);
			return false;
		}

		// Create a window
		m_MainWnd = CreateWindow(
			L"BasicWndClass",
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