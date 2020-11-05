#include "sgpch.h"
#include "Application.h"

#include "LogManager.h"
#include "Utilis/Timestep.h"
#include "Event/Event.h"

namespace SG
{
	std::wstring Application::s_AppName = L"";
	Application* Application::s_Instance = nullptr;
	bool Application::s_IsRunning = true;
	bool Application::s_IsInitialized = false;

	Application::Application(const std::wstring& name)
	{
		SG_CORE_ASSERT(!s_Instance, "Application already exist!");
		s_Instance = this;
		s_AppName = name;
	}

	Application::~Application()
	{
		for (auto layer : m_LayerStack)
			layer->OnDettach();
	}

	bool Application::Init(const HINSTANCE& wndInstance, int show)
	{
		// LogManager initialized
		SG::LogManager::Init();

		WindowProps m_MainWndProps = WindowProps(wndInstance, show);
		m_MainWindow = Window::Create(m_MainWndProps);
		
		if (!m_MainWindow->OnCreate())
			return false;

		if (!s_IsInitialized)
			for (auto layer : m_LayerStack)
				layer->OnAttach();

		s_IsInitialized = true;
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
		if (s_IsInitialized)
			layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer) noexcept
	{
		m_LayerStack.PushOverlay(layer);
		if (s_IsInitialized)
			layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
	}

}