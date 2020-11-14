#include "sgpch.h"
#include "Timer.h"

#include <windows.h>

namespace SG
{

	Timer::Timer(const std::string& name)
		:m_Name(name), m_DeltaTime(-1.0), m_SecondsPerCount(0.0),
		m_BaseTime(0.0), m_PausedTime(0.0), m_StopTime(0.0), m_PrevTime(0.0), m_CurrTime(0.0), m_IsStopped(false)
	{
		uint64_t countsPerSeconds;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSeconds);
		m_SecondsPerCount = 1.0 / (double)countsPerSeconds;
	}

	float Timer::GetTotalTime() const
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance 
		// mStopTime - mBaseTime includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:  
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

		if (m_IsStopped)
		{
			return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
		}

		// The distance mCurrTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from mCurrTime:  
		//
		//  (mCurrTime - mPausedTime) - mBaseTime 
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mCurrTime

		else
		{
			return (float)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
		}
	}

	float Timer::GetDeltaTime() const
	{
		return (float)m_DeltaTime;
	}

	void Timer::Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_BaseTime = currTime;
		m_PrevTime = currTime;
		m_StopTime = 0;
		m_IsStopped = false;
	}

	void Timer::Start()
	{
		// Accumulate the time elapsed between stop and start pairs.
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  mBaseTime       mStopTime        startTime     
		__int64 startTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		if (m_IsStopped)
		{
			m_PausedTime += (startTime - m_StopTime);

			m_PrevTime = startTime;
			m_StopTime = 0;
			m_IsStopped = false;
		}
	}

	void Timer::Stop()
	{
		if (!m_IsStopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			m_StopTime = currTime;
			m_IsStopped = true;
		}
	}

	void Timer::Tick()
	{
		if (m_IsStopped) // If is stopped, we don't Tick
		{
			m_DeltaTime = 0.0;
			return;
		}

		// GetCurrTime
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_CurrTime = currTime;

		// Time difference between this frame and the previous.
		m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;

		// Prepare for next frame.
		m_PrevTime = m_CurrTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (m_DeltaTime < 0.0)
		{
			m_DeltaTime = 0.0;
		}
	}

}