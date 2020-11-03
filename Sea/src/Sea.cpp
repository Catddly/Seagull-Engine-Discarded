#include "Seagull.h"

// -------------- EntryPoint ----------------
#include "Core/EntryPoint.h"
// ------------------------------------------

class MessageLayer : public SG::Layer
{
public:
	MessageLayer() : SG::Layer("MessageLayer") {}

	void OnUpdate() override
	{
		SG_INFO("Message layer!");
		Sleep(1000);
	}
private:
};

class Sea : public SG::Application
{
public:
	Sea() : SG::Application("Sea") 
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
