#pragma once

#include "Core/Layer.h"
#include "Event/Event.h"

namespace SG
{

	class ImGuiLayer : public Layer 
	{
	public:
		ImGuiLayer() : Layer(L"ImGuiLayer") {}
		~ImGuiLayer() = default;

		virtual void OnAttach() {}
		virtual void OnDettach() {}

		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}
	private:
	};

}