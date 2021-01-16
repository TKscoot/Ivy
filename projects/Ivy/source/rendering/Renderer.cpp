#include "ivypch.h"
#include "Renderer.h"


void Ivy::Renderer::Initialize()
{

	EnableDebugMessages();

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	int texture_units;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);

	//InitLoadingScreen();

	mScene = Scene::GetScene();
	mScene->InitializeGUI(mWindow);
	mScene->InitializeScenePass(mWindow);
}

void Ivy::Renderer::Render(float deltaTime)
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mScene->Render(deltaTime, mWindow->GetWindowSize());
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
        Debug::CoreError(message);
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
