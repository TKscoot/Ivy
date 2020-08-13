#include <Ivy.h>
#include <chrono>
#include <string>
#include <sstream>

int main()
{
	using namespace Ivy;
	std::unique_ptr<Engine> engine = std::make_unique<Engine>();
	engine->Initialize(800, 600, "Ivy Sandbox v0.0.1");
	
	while (!engine->ShouldTerminate())
	{
		engine->NewFrame();
	}

	return 0;
}