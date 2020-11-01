#include "LogManager.h"
#include "Application.h"

#include "Platform/WindowsWindow.h"

extern inline SG::Application* CreateApp();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	// use hInstance and nCmdShow to encapsulate WindowsWindow::InitWindowApp()
	SG::Application* app = CreateApp();

	if (!app->Init(hInstance, nCmdShow))
		return 0;

	int value = app->Run();
	if (value == -1)
		return 0;

	app->Shutdown();

	delete app;
}