#pragma once

#include "Core.h"
#include "Event/Event.h"

#include <string>

namespace SG
{
	// Only one application
	class Application
	{
	public:
		Application(const std::string& name = "App");
		Application(const Application&) = delete;
		virtual ~Application() = default;

		void Init();
		void Run();
		void Shutdown();

		void OnEvent(Event& e);

		Application operator=(const Application& app) = delete;
	private:
		static std::string s_AppName;
		static Application* s_Instance;
		static bool s_IsRunning;
	};

	// To be define in client side
	inline Application* CreateApp();
}