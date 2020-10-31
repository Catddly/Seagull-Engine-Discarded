#include "sgpch.h"
#include "Application.h"

#include "LogManager.h"

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

	void Application::Init()
	{
		// LogManager initialized
		SG::LogManager::Init();
	}

	void Application::Run()
	{
		SG_CORE_TRACE("Welcome to Seagull Engine! App : {0}", s_AppName);
		while (s_IsRunning)
		{
		}
	}

	void Application::Shutdown()
	{

	}

	void Application::OnEvent(Event& e)
	{

	}

}