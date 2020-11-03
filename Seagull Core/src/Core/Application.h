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
		virtual ~Application() = default;

		bool Init(const HINSTANCE& wndInstance, int show);
		int Run();
		void Shutdown();

		void PushLayer(Layer* layer) noexcept;
		void PushOverlay(Layer* layer) noexcept;

		virtual void OnEvent(Event& e) {}
		virtual void OnUpdate(Timestep ts) {}

		Application operator=(const Application& app) = delete;
	private:
		static std::string s_AppName;
		static Application* s_Instance;
		static bool s_IsRunning;

		Scope<Window> m_MainWindow;
		WindowProps m_MainWndProps;

		LayerStack m_LayerStack;
	};

	// To be define in client side
	inline Application* CreateApp();
}