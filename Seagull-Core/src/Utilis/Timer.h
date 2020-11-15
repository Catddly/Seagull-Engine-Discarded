#pragma once

#include <string>

namespace SG
{

	class Timer
	{
	public:
		Timer(const std::string& name);

		float GetTotalTime()const; // in seconds
		float GetDeltaTime()const; // in seconds

		void Reset(); // Call before message loop.
		void Start(); // Call when unpaused.
		void Stop();  // Call when paused.
		void Tick();  // Call every frame.
	private:
		std::string m_Name;

		double m_DeltaTime;
		double m_SecondsPerCount;

		__int64 m_BaseTime;       // timer start time
		__int64 m_PausedTime;     // total time we paused
		__int64 m_StopTime;       // last time when we stopped
		__int64 m_PrevTime;       // last frame duration
		__int64 m_CurrTime;       // curr frame duration

		bool m_IsStopped;
	};

}