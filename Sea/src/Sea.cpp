#include "Seagull.h"

// -------------- EntryPoint ----------------
#include "Core/EntryPoint.h"
// ------------------------------------------

class MessageLayer : public SG::Layer
{
public:
	MessageLayer() : SG::Layer("MessageLayer") {}

	void OnAttach() override
	{
		SG::Renderer::Init();
	}

	void OnEvent(SG::Event& e) override
	{
	}

	void OnUpdate() override
	{
		SG::RenderCommand::Clear();
	}
private:
};

class Sea : public SG::Application
{
public:
	Sea() : SG::Application(L"Sea") 
	{
		PushLayer(new MessageLayer());
	}
	~Sea() = default;
private:
};

SG::Application* CreateApp()
{
	return new Sea();
}
