#include "LogManager.h"
#include "Application.h"

extern inline SG::Application* CreateApp();

int main(int argc, char** argv)
{
	SG::Application* app = CreateApp();

	app->Init();
	app->Run();
	app->Shutdown();

	delete app;

	return 0;
}