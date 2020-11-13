#pragma once

#include "Core.h"
#include "Event/Event.h"
#include "utilis/Timestep.h"
#include "Window.h"

#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"

#include "Event/MouseEvent.h"
#include "Event/KeyboardEvent.h"
#include "Event/ApplicationEvent.h"

#include <string>

namespace SG
{
	using namespace Utils;
	// Only one application
	class Application
	{
	public:
		Application(const std::wstring& name = L"App");
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
		Window* GetWindow() { return m_MainWindow.get(); }
	private:
		bool OnMousePressed(MouseButtonPressedEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		static std::wstring s_AppName;
		static Application* s_Instance;
		static bool s_IsRunning;
		static bool s_IsMinimized;
		static bool s_IsInitialized;
		static bool s_EnableEventLog;

		Scope<Window> m_MainWindow;
		WindowProps m_MainWndProps;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;
	};

	// To be define in client side
	inline Application* CreateApp();
}