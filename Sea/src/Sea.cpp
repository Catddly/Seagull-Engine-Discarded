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
		SG::Renderer::Init();
	}

	void OnEvent(SG::Event& e) override
	{
	}

	void OnImGuiRender() override
	{
		ImGui::ShowDemoWindow();
	}

	void OnUpdate() override
	{
		//SG::RenderCommand::Clear();
	}
private:
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
