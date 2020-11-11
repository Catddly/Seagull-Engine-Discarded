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
	bool Application::s_IsMinimized = false;
	bool Application::s_IsInitialized = false;
	bool Application::s_EnableEventLog = false;

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
		m_MainWindow->SetEventCallbackFn(SG_BIND_EVENT_FUNC(Application::OnEvent));
			
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
		while (s_IsRunning)
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
				if (!s_IsMinimized)
				{
					for (auto layer : m_LayerStack)
					{
						layer->OnUpdate();
					}

					m_MainWindow->OnUpdate();
				}
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
		dispatcher.Dispatch<MouseButtonPressedEvent>(SG_BIND_EVENT_FUNC(Application::OnMousePressed));
		dispatcher.Dispatch<KeyPressedEvent>(SG_BIND_EVENT_FUNC(Application::OnKeyPressed));
		dispatcher.Dispatch<MouseMovedEvent>(SG_BIND_EVENT_FUNC(Application::OnMouseMoved));
		dispatcher.Dispatch<WindowCloseEvent>(SG_BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(SG_BIND_EVENT_FUNC(Application::OnWindowResize));

		if (s_EnableEventLog)
			SG_CORE_TRACE("{0}", e);

		for (auto end = m_LayerStack.end(); end != m_LayerStack.begin(); )
		{
			if (e.IsHandled())
				break;
			(*--end)->OnEvent(e);
		}
	}

	bool Application::OnMousePressed(MouseButtonPressedEvent& e)
	{
		return false;
	}

	bool Application::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetKeycode() == 'M')
			s_EnableEventLog = !s_EnableEventLog;
		return false;
	}

	bool Application::OnMouseMoved(MouseMovedEvent& e)
	{
		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		auto wnd = static_cast<HWND>(e.GetNativeWindowHandle());
		DestroyWindow(wnd);
		PostQuitMessage(0);
		s_IsRunning = false;
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			s_IsMinimized = true;
			return false;
		}

		return false;
	}

}