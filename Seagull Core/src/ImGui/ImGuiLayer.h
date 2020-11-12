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

		virtual void OnAttach() override;
		virtual void OnDettach() override;

		virtual void OnUpdate() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();
	private:
	};

}