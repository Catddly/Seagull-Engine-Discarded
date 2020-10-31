#include "sgpch.h"

#include "LogManager.h"

namespace SG
{
	Ref<spdlog::logger> LogManager::s_CoreLogger;
	Ref<spdlog::logger> LogManager::s_ClientLogger;

	void LogManager::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("Core");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("CLIENT");
		s_ClientLogger->set_level(spdlog::level::trace);
	}

}