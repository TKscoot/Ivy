#include "ivypch.h"
#include "Engine.h"

Ivy::Engine::Engine()
{
}

Ivy::Engine::~Engine()
{
}

void Ivy::Engine::Initialize(int windowWidth, int  windowHeight, const std::string& windowTitle)
{
	// creating window
	mWnd = CreatePtr<Window>(windowWidth, windowHeight, windowTitle);
	if (mWnd == nullptr)
	{
		std::cout << "Failed to create window" << std::endl;
		throw;
	}

	// initializing glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw;
	}

	Debug::Initialize();

	Input::Initialize(mWnd);

	Debug::CoreInfo(" *** Engine initialized! *** ");
}

void Ivy::Engine::Run()
{
}

void Ivy::Engine::NewFrame()
{
	static auto lastTime = std::chrono::high_resolution_clock::now();

	if (!mWnd)
	{
		throw "You need to create window first!";
	}

	auto newtime = std::chrono::high_resolution_clock::now();
	lastTime = std::chrono::high_resolution_clock::now();

	// Please note: this is old, OpenGL 1.1 code. It's here for simplicity.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.5f, -0.5f);
	glVertex2f(0, 0.5f);
	glEnd();

	mWnd->SwapBuffers();

	mWnd->PollEvents();

	if (Input::IsKeyDown(W) && Input::IsKeyDown(Q))
	{
		Debug::CoreLog("W and Q pressed");
	}
	if (Input::IsKeyDown(W))
	{
		Debug::CoreLog("W pressed");
	}
	if (Input::IsKeyDown(Q))
	{
		Debug::CoreLog("Q pressed");
	}

	newtime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> delta = newtime - lastTime;

	std::ostringstream ss;
	ss << "Ivy Sandbox v0.0.1 | frame time: ";
	ss << delta.count();
	ss << " | FPS: ";
	ss << (1 / delta.count());

	mWnd->SetTitle(ss.str());

	newtime = lastTime;
}

bool Ivy::Engine::ShouldTerminate()
{
	if(mWnd->ShouldClose())
	{
		return true;
	}

	return false;
}

