#pragma once

#include "Event/Event.h"

#include <string>

namespace SG
{

	class Layer
	{
	public:
		Layer(const std::wstring& name)
			:m_Name(name) {}
		virtual ~Layer() = default;

		inline const std::wstring& GetName() const { return m_Name; }

		virtual void OnAttach() {}
		virtual void OnDettach() {}

		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}
	private:
		std::wstring m_Name;
	};

}