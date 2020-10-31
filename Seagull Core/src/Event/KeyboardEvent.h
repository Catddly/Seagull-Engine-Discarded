#pragma once

#include "Event.h"

namespace SG
{

	class KeyEvent : public Event
	{
	public:
		inline int GetKeycode() const { return m_Keycode; }

		EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyBoard)
	protected:
		KeyEvent(int keycode)
			:m_Keycode(keycode) {}
		KeyEvent(const KeyEvent&) = delete;
		~KeyEvent() = default;

		KeyEvent operator=(const KeyEvent& rhs) = delete;

		int m_Keycode; // TODO : replace to SG keycode
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			:KeyEvent(keycode), m_RepeatCount(repeatCount) {}
		KeyPressedEvent(const KeyPressedEvent&) = delete;
		~KeyPressedEvent() = default;

		inline int GetRepeatCount() const { return m_RepeatCount; }

		EVENT_TYPE(KeyPressed)

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_Keycode << "(" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		KeyPressedEvent operator=(const KeyPressedEvent& rhs) = delete;
	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			:KeyEvent(keycode) {}
		KeyReleasedEvent(const KeyPressedEvent&) = delete;
		~KeyReleasedEvent() = default;

		EVENT_TYPE(KeyReleased)

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_Keycode;
			return ss.str();
		}

		KeyReleasedEvent operator=(const KeyReleasedEvent& rhs) = delete;
	private:
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			:KeyEvent(keycode) {}
		KeyTypedEvent(const KeyTypedEvent&) = delete;
		~KeyTypedEvent() = default;

		EVENT_TYPE(KeyTyped)

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_Keycode;
			return ss.str();
		}

		KeyTypedEvent operator=(const KeyTypedEvent& rhs) = delete;
	private:
	};

}