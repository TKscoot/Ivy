#include "ivypch.h"
#include "Renderer.h"


void Ivy::Renderer::Initialize()
{

	EnableDebugMessages();
	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);


	//glEnable(GL_MULTISAMPLE);

	int texture_units;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);

	InitLoadingScreen();

	AddShaderIncludes();

	auto smInst = SceneManager::GetInstance();
	smInst->SetWindow(mWindow);
	//mScene->InitializeGUI(mWindow);
	//mScene->InitializeScenePass(mWindow);

	mImGuiHook = CreatePtr<ImGuiHook>(mWindow);

	SceneManager::GetInstance()->RegisterSceneLoadCallback([&](Ptr<Scene> scene) {
		if(scene)
		{
			mScene = scene;
		}
		else
		{
			Debug::Error("Could not register scene load callback! Scene is invalid!");
		}

	});
}

void Ivy::Renderer::Render(float deltaTime)
{

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mScene->Render(deltaTime, mWindow->GetWindowSize());

	mImGuiHook->Render();
}

void Ivy::Renderer::GLLogCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char* message,
                                  const void* userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        Debug::CoreCritical(message);
        return;
    case GL_DEBUG_SEVERITY_MEDIUM:
        Debug::CoreWarning(message);
        return;
    case GL_DEBUG_SEVERITY_LOW:
        Debug::CoreWarning(message);
        return;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        Debug::CoreLog(message);
        return;
    }
}

void Ivy::Renderer::EnableDebugMessages()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLLogCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
}

void Ivy::Renderer::InitLoadingScreen()
{
	float vertices[] = {
		// pos        // tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};;

	BufferLayout layout =
	{
		{ShaderDataType::Float2, "aPos", 0},
		{ShaderDataType::Float2, "aCoords", 0}
	};

	mLoadingScreenShader  = CreatePtr<Shader>("shaders/LoadingScreen.vert", "shaders/LoadingScreen.frag");
	mLoadingScreenTexture = CreatePtr<Texture2D>("assets/textures/Misc/LoadingScreen.png");

	mVertexArray  = CreatePtr<VertexArray>(layout);
	mVertexBuffer = CreatePtr<VertexBuffer>();
	mVertexArray->SetVertexBuffer(mVertexBuffer);
	mVertexArray->Bind();
	mVertexBuffer->SetBufferData(&vertices, sizeof(vertices));

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, mWindow->GetWindowSize().x, mWindow->GetWindowSize().y);

	mLoadingScreenShader->Bind();
	mVertexArray->Bind();
	mLoadingScreenTexture->Bind(0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glfwSwapBuffers(mWindow->GetHandle());

	mLoadingScreenShader->Unbind();
	mVertexArray->Unbind();

}

bool endsWith(std::string const &fullString, std::string const &ending)
{
	if(fullString.length() >= ending.length())
	{
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else
	{
		return false;
	}
}

void getFilenameFromPath(std::string& str)
{
	std::size_t found = str.find_last_of("/\\");
	str = str.substr(found + 1);
}

void Ivy::Renderer::AddShaderIncludes()
{

	for(auto& p : std::filesystem::recursive_directory_iterator("shaders\\include"))
	{
		String path = p.path().string();
		if(endsWith(path, ".glsl") 
			|| endsWith(path, ".inc") 
			|| endsWith(path, ".shaderinc") 
			|| endsWith(path, ".glslinc"))
		{
			size_t pos = path.find("shaders\\include\\");
			String name = path;
			getFilenameFromPath(name);

			String source = "";

			// Open file
			std::ifstream fs(path.c_str());
			if(!fs)
			{
				Debug::CoreError("Could not read shader include file: {}", path);
			}

			// Iterate through file and save result in string
			source.assign((std::istreambuf_iterator<char>(fs)),
				(std::istreambuf_iterator<char>()));

			name.insert(0, "/");
			
			glNamedStringARB(GL_SHADER_INCLUDE_ARB, name.size(), name.c_str(), source.size(), source.c_str());
			Debug::CoreInfo("Added {} to shader includes", name);

		}
	}
}
