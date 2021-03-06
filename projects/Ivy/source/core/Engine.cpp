#include "ivypch.h"
#include "Engine.h"

Ivy::Engine::Engine()
{
}

Ivy::Engine::~Engine()
{
}

// Ensure to use dedicated GPU
#ifdef _WIN32
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#elif __linux__ 
// TODO: Ensure to use dedicated GPU on linux
#endif

void ErrorCallback(int, const char* err_str)
{
	Ivy::Debug::CoreError("GLFW Error: {}", err_str);
}

void Ivy::Engine::Initialize(int windowWidth, int  windowHeight, const std::string& windowTitle)
{
    glfwSetErrorCallback(ErrorCallback);
	Debug::Initialize();

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


	CheckGLVersion(4, 0);

	Input::Initialize(mWnd);
	Input::SetMouseCursorVisible(false);

	mRenderer = CreatePtr<Renderer>(mWnd);
	mRenderer->Initialize();

	SceneManager::GetInstance()->RegisterSceneLoadCallback([&](Ptr<Scene> scene) {
		if(scene)
		{
			mCurrentScene = scene;
		}
		else
		{
			Debug::Error("Could not register scene load callback! Scene is invalid!");
		}

	});

	SceneManager::GetInstance()->RegisterSceneCreateCallback([&](Ptr<Scene> scene) {
		if (scene)
		{
			mScenes.push_back(scene);
		}
		else
		{
			Debug::Error("Could not register scene create callback! Scene is invalid!");
		}

		});


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

	//if (!mWnd)
	//{
	//	throw "You need to create a window first!";
	//}

	auto newtime = std::chrono::high_resolution_clock::now();
	lastTime = std::chrono::high_resolution_clock::now();

	mWnd->PollEvents();

	if(!mWnd->IsIconified())
	{
		mRenderer->NotifyImGuiNewFrame();

		mCurrentScene->Update(mDeltaTime);
		
		mRenderer->Render(mDeltaTime);

		mWnd->SwapBuffers();
	}

	newtime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> delta = newtime - lastTime;
	mDeltaTime = delta.count();

	static float timer = 0;
	timer += mDeltaTime;

	if (timer >= 1.0f)
	{
		std::ostringstream ss;
		ss << "Ivy Sandbox v0.0.1 | frame time: ";
		ss << delta.count() * 1000;
		ss << " | FPS: ";
		ss << (1 / delta.count());
		mWnd->SetTitle(ss.str());
		
		timer = 0.0f;
	}

	newtime = lastTime;
}

bool Ivy::Engine::ShouldTerminate()
{
	if(mTerminate || mWnd->ShouldClose())
	{
		mWnd->Finalize();
		std::cin.get();
		exit(-1);
		return true;
	}

	return false;
}

void Ivy::Engine::CheckGLVersion(int supportedMajor, int supportedMinor)
{
	int major = 0, minor = 0;
	major = GLVersion.major;
	minor = GLVersion.minor;

	if(major == 0 && minor == 0)
	{
		Debug::CoreError("{}.{} -- OpenGL version is zero! Well try to run anyway!", major, minor);
		return;
	}

	if(major < supportedMajor)
	{
		Debug::CoreCritical("{}.{} -- OpenGL version not supported! Engine will shutdown!\n (only versions >= {}.{} are supported)", 
			major, minor, supportedMajor, supportedMinor);
		mTerminate = true;
		ShouldTerminate();
		return;
	}
	else if(minor < supportedMinor)
	{
		Debug::CoreCritical("{}.{} -- OpenGL version not supported! Engine will shutdown!\n (only versions >= {}.{} are supported)", 
			major, minor, supportedMajor, supportedMinor);
		mTerminate = true;
		ShouldTerminate();
		return;
	}

	Debug::CoreInfo("{}.{} -- OpenGL version supported!", major, minor);
}

