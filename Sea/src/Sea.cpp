#include "Seagull.h"

// -------------- EntryPoint ----------------
#include "Core/EntryPoint.h"
// ------------------------------------------

class Sea : public SG::Application
{
public:
	Sea() 
		: SG::Application("Sea") {}
	~Sea() = default;
	
private:
};

SG::Application* CreateApp()
{
	return new Sea();
}
