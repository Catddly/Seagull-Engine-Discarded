#include "Seagull.h"

// -------------- EntryPoint ----------------
#include "Core/EntryPoint.h"
// ------------------------------------------

class DemoLayer : public SG::Layer
{
public:
	DemoLayer() : SG::Layer(L"DemoLayer") {}

	void OnAttach() override
	{
	}

	void OnEvent(SG::Event& e) override
	{
	}

	void OnImGuiRender() override
	{
//#pragma region Imgui::DockSpace
//		static bool p_open = true;
//		static bool opt_fullscreen_persistant = true;
//		bool opt_fullscreen = opt_fullscreen_persistant;
//		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
//
//		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
//		// because it would be confusing to have two docking targets within each others.
//		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
//		if (opt_fullscreen)
//		{
//			ImGuiViewport* viewport = ImGui::GetMainViewport();
//			ImGui::SetNextWindowPos(viewport->GetWorkPos());
//			ImGui::SetNextWindowSize(viewport->GetWorkSize());
//			ImGui::SetNextWindowViewport(viewport->ID);
//			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//			// Always be the top of the view
//			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
//			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
//		}
//
//		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
//		// and handle the pass-thru hole, so we ask Begin() to not render a background.
//		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
//			window_flags |= ImGuiWindowFlags_NoBackground;
//
//		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
//		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
//		// all active windows docked into it will lose their parent and become undocked.
//		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
//		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
//		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//
//		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
//		ImGui::PopStyleVar();
//		if (opt_fullscreen)
//			ImGui::PopStyleVar(2);
//
//		// DockSpace
//		ImGuiIO& io = ImGui::GetIO();
//		ImGuiStyle& style = ImGui::GetStyle();
//		float windowMinSizeX = style.WindowMinSize.x;
//		style.WindowMinSize.x = 400.0f;
//
//		// hold shift to dock
//		io.ConfigDockingWithShift = true;
//		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
//		{
//			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
//			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
//		}
//
//		style.WindowMinSize.x = windowMinSizeX;
//#pragma endregion Imgui::DockSpace

		static float interval = 0.0f;
		static float fps = 0.0f, mspf = 0.0f;
		interval += m_Dt;
		if (interval >= 0.5f)
		{
			fps = 1.0f / m_Dt;
			mspf = 1000.0f / fps;
			interval = 0.0f;
		}

		ImGui::ShowDemoWindow();

		ImGui::Begin("Test");
		ImGui::Text("Hello!");
		ImGui::Text("%.2f %.2f", SG::Input::GetMouseX(), SG::Input::GetMouseY());
		ImGui::Text("Fps: %.3f, mspf: %.3f(ms)", fps, mspf);
		ImGui::End();

		//ImGui::End(); //Dock Space
	}

	void OnUpdate(SG::DeltaTime dt) override
	{
		m_Dt = dt;
		//SG::RenderCommand::Clear();
	}
private:
	SG::DeltaTime m_Dt = 0.0f;
};

class Sea : public SG::Application
{
public:
	Sea() : SG::Application(L"Sea") 
	{
		PushLayer(new DemoLayer());
	}
	~Sea() = default;
private:
};

SG::Application* CreateApp()
{
	return new Sea();
}
