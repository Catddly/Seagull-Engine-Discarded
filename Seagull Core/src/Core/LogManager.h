#pragma once

#include "Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace SG
{
	// Only one global logger
	class LogManager
	{
	public:
		LogManager() = default;
		~LogManager() = default;

		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger()   { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

// core logging macros
#define SG_CORE_INFO(...)     SG::LogManager::GetCoreLogger()->info(__VA_ARGS__);
#define SG_CORE_TRACE(...)    SG::LogManager::GetCoreLogger()->trace(__VA_ARGS__);
#define SG_CORE_WARN(...)     SG::LogManager::GetCoreLogger()->warn(__VA_ARGS__);
#define SG_CORE_ERROR(...)    SG::LogManager::GetCoreLogger()->error(__VA_ARGS__);
#define SG_CORE_CRITICAL(...) SG::LogManager::GetCoreLogger()->critical(__VA_ARGS__);

// client logging macros
#define SG_INFO(...)     SG::LogManager::GetClientLogger()->info(__VA_ARGS__);
#define SG_TRACE(...)    SG::LogManager::GetClientLogger()->trace(__VA_ARGS__);
#define SG_WARN(...)     SG::LogManager::GetClientLogger()->warn(__VA_ARGS__);
#define SG_ERROR(...)    SG::LogManager::GetClientLogger()->error(__VA_ARGS__);
#define SG_CRITICAL(...) SG::LogManager::GetClientLogger()->critical(__VA_ARGS__);

}