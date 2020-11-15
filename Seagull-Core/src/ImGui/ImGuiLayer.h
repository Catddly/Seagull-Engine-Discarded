#pragma once

#include "Core/Layer.h"

#include "Event/Event.h"
#include "Event/KeyboardEvent.h"
#include "Event/MouseEvent.h"
#include "Event/ApplicationEvent.h"

#include "Utilis/DeltaTime.h"

namespace SG
{

	class ImGuiLayer : public Layer 
	{
	public:
		ImGuiLayer() : Layer(L"ImGuiLayer") {}
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDettach() override;

		virtual void OnUpdate(DeltaTime dt) override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();
	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnKeyReleased(KeyReleasedEvent& e);
		bool OnKeyTyped(KeyTypedEvent& e);
		bool OnMouseFocusWindowChanged(MouseFocusWindowChangedEvent& e);
		bool OnDeviceChanged(DeviceChangedEvent& e);
	private:
		friend class WindowsWindow;
	};

}