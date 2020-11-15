#pragma once

#include <chrono>
#include <thread>

namespace SG
{

	class Profiler
	{
	public:
		Profiler() noexcept
			:m_IsStoped(false)
		{
			m_StartTime = std::chrono::high_resolution_clock::now();
		}

		~Profiler() noexcept
		{
			if (!m_IsStoped)
				Stop();
		}

		void Stop();
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
		bool m_IsStoped;
	};

}