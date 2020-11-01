#include "sgpch.h"
#include "Profiler.h"

namespace SG
{

	namespace Utils
	{

		void Profiler::Stop()
		{
			auto now = std::chrono::high_resolution_clock::now();

			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count();

			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			m_IsStoped = true;
		}

	}

}