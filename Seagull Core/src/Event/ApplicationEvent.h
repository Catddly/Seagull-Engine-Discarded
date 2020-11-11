#pragma once

#include <stdint.h>

#include "Event.h"
#include "Core/Window.h"

namespace SG
{

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			:m_Width(width), m_Height(height) {}
		WindowResizeEvent(const WindowResizeEvent&) = delete;
		~WindowResizeEvent() = default;

		inline uint32_t GetWidth()  const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_TYPE(WindowResize)
		EVENT_CATEGORY(EventCategoryApplication)

		WindowResizeEvent operator=(const WindowResizeEvent& rhs) = delete;
	private:
		uint32_t m_Width, m_Height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent(Window* wnd) :m_ClosedWindow(wnd) {}

		void* GetNativeWindowHandle() const { return m_ClosedWindow->GetNativeWindow(); }

		EVENT_TYPE(WindowClose)
		EVENT_CATEGORY(EventCategoryApplication)
	private:
		Window* m_ClosedWindow;
	};

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_TYPE(AppTick)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_TYPE(AppUpdate)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_TYPE(AppRender)
		EVENT_CATEGORY(EventCategoryApplication)
	};
}