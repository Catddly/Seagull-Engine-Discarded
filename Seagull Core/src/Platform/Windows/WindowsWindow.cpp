#include "sgpch.h"
#include "WindowsWindow.h"

namespace SG
{
	// WPARAM 和 LPARAM 是消息响应机制
	LRESULT CALLBACK WindowProcess(HWND hWnd /*window*/, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_LBUTTONDOWN:
		{
			MessageBox(0, L"你好，小伙子", L"Hello", MB_OK);
			return 0;
		}
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				DestroyWindow(hWnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0); return 0;
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