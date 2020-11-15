#pragma once

#include "Event.h"

namespace SG
{

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetKeycode() const { return m_Button; }

		EVENT_CATEGORY(EventCategoryInput | EventCategoryMouseButton)
	protected:
		MouseButtonEvent(int button)
			:m_Button(button) {}
		MouseButtonEvent(const MouseButtonEvent&) = delete;
		~MouseButtonEvent() = default;

		MouseButtonEvent operator=(const MouseButtonEvent& rhs) = delete;

		int m_Button; // TODO : replace to SG button
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float x, float y)
			:m_ScrollX(x), m_ScrollY(y) {}
		MouseScrolledEvent(const MouseScrolledEvent&) = delete;
		~MouseScrolledEvent() = default;

		inline float GetScrollX() const { return m_ScrollX; }
		inline float GetScrollY() const { return m_ScrollY; }

		EVENT_TYPE(MouseScrolled)
		EVENT_CATEGORY(EventCategoryMouse | EventCategoryInput)

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_ScrollX << ", " << m_ScrollY;
			return ss.str();
		}

		MouseScrolledEvent operator=(const MouseButtonEvent& rhs) = delete;
	private:
		float m_ScrollX, m_ScrollY;
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			:m_MouseX(x), m_MouseY(y) {}
		MouseMovedEvent(const MouseMovedEvent&) = delete;
		~MouseMovedEvent() = default;

		inline float GetMouseX() const { return m_MouseX; }
		inline float GetMouseY() const { return m_MouseY; }

		EVENT_TYPE(MouseMoved)
		EVENT_CATEGORY(EventCategoryMouse | EventCategoryInput)

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		MouseMovedEvent operator=(const MouseButtonEvent& rhs) = delete;
	private:
		float m_MouseX, m_MouseY;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button, int repeatCount)
			:MouseButtonEvent(button), m_repeatCount(repeatCount) {}
		MouseButtonPressedEvent(const MouseButtonPressedEvent&) = delete;
		~MouseButtonPressedEvent() = default;

		EVENT_TYPE(MouseButtonPressed)
		EVENT_CATEGORY(EventCategoryMouseButton | EventCategoryInput)

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		MouseButtonPressedEvent operator=(const MouseButtonPressedEvent& rhs) = delete;
	private:
		int m_repeatCount = 0;
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			:MouseButtonEvent(button) {}
		MouseButtonReleasedEvent(const MouseButtonReleasedEvent&) = delete;
		~MouseButtonReleasedEvent() = default;

		EVENT_TYPE(MouseButtonReleased)
		EVENT_CATEGORY(EventCategoryMouseButton | EventCategoryInput)

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		MouseButtonReleasedEvent operator=(const MouseButtonReleasedEvent& rhs) = delete;
	private:
	};

	class MouseFocusWindowChangedEvent : public Event
	{
	public:
		MouseFocusWindowChangedEvent(void* wnd, WORD pos, WORD type)
			:m_NativeWindow(wnd), m_HitCursorPosition(pos), m_EventTriggerType(type) {}
		MouseFocusWindowChangedEvent(const MouseFocusWindowChangedEvent&) = delete;
		~MouseFocusWindowChangedEvent() = default;

		inline void* GetNativeWindow() const { return m_NativeWindow; }
		inline WORD GetCursorPosition() const { return m_HitCursorPosition; }
		inline WORD GetEventTriggerType() const { return m_EventTriggerType; }

		EVENT_TYPE(MouseFocusWindowChanged)
		EVENT_CATEGORY(EventCategoryMouse | EventCategoryInput)

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseFocusWindowChangedEvent";
			return ss.str();
		}

		MouseFocusWindowChangedEvent operator=(const MouseFocusWindowChangedEvent& rhs) = delete;
	private:
		void* m_NativeWindow;
		WORD m_HitCursorPosition;
		WORD m_EventTriggerType;
	};

}