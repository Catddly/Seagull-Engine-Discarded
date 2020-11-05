#include "LogManager.h"
#include "Application.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Platform/DirectX/DirectXHelper.h"

extern inline SG::Application* CreateApp();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
#if defined(SG_DEBUG) & (defined(DEBUG) | defined(_DEBUG))
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	// use hInstance and nCmdShow to encapsulate WindowsWindow::InitWindowApp()
#if defined(SG_DEBUG) & (defined(DEBUG) | defined(_DEBUG))
	try {
#endif
		SG::Application* app = CreateApp();

		if (!app->Init(hInstance, nCmdShow))
			return 0;

		int value = app->Run();
		if (value == -1)
			return 0;

		app->Shutdown();
		delete app;
#if defined(SG_DEBUG) & (defined(DEBUG) | defined(_DEBUG))
	}
	catch (SG::DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
#endif
}