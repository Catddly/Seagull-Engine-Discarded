#pragma once

#include <string>
#include <sstream>

namespace SG
{

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryInput =       1 << 1,
		EventCategoryKeyBoard =    1 << 2,
		EventCategoryMouse =       1 << 3,
		EventCategoryMouseButton = 1 << 4
	};

#define EVENT_TYPE(type) static EventType GetStaticType() const { return EventType::##type; } \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

#define EVENT_CATEGORY(category) virtual int GetCategoryFlag() const override { (int)category; }

	class Event
	{
	public:
		Event() = default;
		virtual ~Event() = default;

		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlag() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory cat) { return cat & GetCategoryFlag(); }
		inline bool IsHandled(Event e) { return m_IsHandled; }

	protected:
		bool m_IsHandled = false;
	};

	class EventDispatcher
	{
	public:
		template <typename T>
		using EventFunc = bool(*)(T);

		EventDispatcher(Event& e)
			:m_Event(e) {}
		~EventDispatcher() = default;

		template <typename T>
		bool Dispatch(EventFunc<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_IsHandled = func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}