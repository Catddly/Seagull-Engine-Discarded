#pragma once

namespace SG
{

	class DeltaTime
	{
	public:
		DeltaTime(float dt)
			:m_DeltaTime(dt) {}
		~DeltaTime() = default;

		float GetSecond() const { return m_DeltaTime; }
		float GetMilliSecond() const { return m_DeltaTime * 1000.0f; }

		operator float() const { return m_DeltaTime; }
	private:
		float m_DeltaTime;
	};

}