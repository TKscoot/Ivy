#include "ivypch.h"
#include "Engine.h"

Ivy::Engine::Engine()
{
}

Ivy::Engine::~Engine()
{
}

void ErrorCallback(int, const char* err_str)
{
	Ivy::Debug::CoreError("GLFW Error: {}", err_str);
}

void Ivy::Engine::Initialize(int windowWidth, int  windowHeight, const std::string& windowTitle)
{
    glfwSetErrorCallback(ErrorCallback);
	// creating window
	mWnd = CreatePtr<Window>(windowWidth, windowHeight, windowTitle);
	if (mWnd == nullptr)
	{
		Debug::CoreError("Failed to create window!");
	}
	mWnd->EnableVsync(false);

	// initializing glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Debug::CoreError("Failed to initialize GLAD!");
	}

	Debug::Initialize();

	Input::Initialize(mWnd);
	Input::SetMouseCursorVisible(false);

	mRenderer = CreatePtr<Renderer>(mWnd);
	mRenderer->Initialize();

	Debug::CoreInfo(" *** Engine initialized! *** ");
}

void Ivy::Engine::Run()
{
	while (!ShouldTerminate())
	{
		NewFrame();
	}
}

void Ivy::Engine::NewFrame()
{
	static auto lastTime = std::chrono::high_resolution_clock::now();

	if (!mWnd)
	{
		throw "You need to create a window first!";
	}

	auto newtime = std::chrono::high_resolution_clock::now();
	lastTime = std::chrono::high_resolution_clock::now();

	mWnd->PollEvents();

	mRenderer->Render();

	mWnd->SwapBuffers();

	newtime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> delta = newtime - lastTime;
	mDeltaTime = delta.count();

	static float timer = 0;
	timer += mDeltaTime;

	if (timer >= 1.0f)
	{
		std::ostringstream ss;
		ss << "Ivy Sandbox v0.0.1 | frame time: ";
		ss << delta.count();
		ss << " | FPS: ";
		ss << (1 / delta.count());
		mWnd->SetTitle(ss.str());
		
		timer = 0.0f;
	}


	newtime = lastTime;
}

bool Ivy::Engine::ShouldTerminate()
{
	if(mWnd->ShouldClose())
	{
		mWnd->Finalize();
		return true;
	}

	return false;
}

