#pragma once

#include "Event/Event.h"

#include <string>

namespace SG
{

	class Layer
	{
	public:
		Layer(const std::string& name)
			:m_Name(name) {}
		virtual ~Layer() = default;

		inline const std::string& GetName() const { return m_Name; }

		virtual void OnAttach() noexcept {}
		virtual void OnDettach() noexcept {}

		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}
	private:
		std::string m_Name;
	};

}