#include "sgpch.h"
#include "Application.h"

#include "LogManager.h"
#include "utilis/Timestep.h"

namespace SG
{
	std::string Application::s_AppName = "";
	Application* Application::s_Instance = nullptr;
	bool Application::s_IsRunning = true;

	Application::Application(const std::string& name)
	{
		SG_CORE_ASSERT(!s_Instance, "Application already exist!");
		s_Instance = this;
		s_AppName = name;
	}

	bool Application::Init(const HINSTANCE& wndInstance, int show)
	{
		// LogManager initialized
		SG::LogManager::Init();

		WindowProps m_MainWndProps = WindowProps(wndInstance, show);
		m_MainWindow = Window::Create(m_MainWndProps);
		
		if (!m_MainWindow->OnCreate())
			return false;

		return true;
	}

	int Application::Run()
	{
		MSG msg = {0};

		// until receive WM_QUIT, if will keep looping
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				// If there are any window messages, then process them
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// main game loop
				for (auto layer : m_LayerStack)
				{
					layer->OnUpdate();
				}

				m_MainWindow->OnUpdate();
			}

		}

		return (int)msg.wParam;
	}

	void Application::Shutdown()
	{
	}

	void Application::PushLayer(Layer* layer) noexcept
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer) noexcept
	{
		m_LayerStack.PushOverlay(layer);
	}

}