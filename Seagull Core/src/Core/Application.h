#pragma once

#include "Core.h"
#include "Event/Event.h"
#include "utilis/Timestep.h"
#include "Window.h"

#include "LayerStack.h"

#include <string>

namespace SG
{
	using namespace Utils;
	// Only one application
	class Application
	{
	public:
		Application(const std::string& name = "App");
		Application(const Application&) = delete;
		Application operator=(const Application& app) = delete;
		virtual ~Application();

		bool Init(const HINSTANCE& wndInstance, int show);
		int Run();
		void Shutdown();

		void PushLayer(Layer* layer) noexcept;
		void PushOverlay(Layer* layer) noexcept;

		void OnEvent(Event& e);

		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *m_MainWindow; }
	private:
		static std::string s_AppName;
		static Application* s_Instance;
		static bool s_IsRunning;
		static bool s_IsInitialized;

		Scope<Window> m_MainWindow;
		WindowProps m_MainWndProps;

		LayerStack m_LayerStack;
	};

	// To be define in client side
	inline Application* CreateApp();
}