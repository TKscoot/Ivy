#include "ivypch.h"
#include "Renderer.h"


void Ivy::Renderer::Initialize()
{
	EnableDebugMessages();

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mScene = Scene::GetScene();

	mCamera = CreatePtr<Camera>(Vec3(0.0f, 0.0f, 0.0f));
}

void Ivy::Renderer::Render()
{
    float timeValue = glfwGetTime();

	static float lastFrame = 0.0f;
	float dt = timeValue - lastFrame;
	lastFrame = timeValue;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    float colorValue = (sin(timeValue) / 2.0f) + 0.5f;

	Mat4 model = Mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

	mCamera->Update(dt);
	Mat4 view = mCamera->GetViewMatrix();

	Vec2 currentWindowSize = mWindow->GetWindowSize();
    Mat4 projection = glm::perspective(glm::radians(45.0f), currentWindowSize.x / currentWindowSize.y, 0.1f, 1000.0f);

	for(auto& entity : mScene->GetEntities())
	{
		for (auto& mat : entity->GetComponentsOfType<Material>())
		{
			Ptr<Shader> shader = mat->GetShader();

			// Bind Textures to specific slot (diff, norm, ...)
			for (auto& kv : mat->GetTextures())
			{
				kv.second->Bind(static_cast<uint32_t>(kv.first));
			}

			// TODO: Compare if this shader is the default shader
			if (shader->GetRendererID() != Shader::GetCurrentlyUsedShaderID())
			{
				shader->Bind();
			}

			shader->SetUniformMat4("view", view);
			shader->SetUniformMat4("projection", projection);
			shader->SetUniformFloat3("viewPos", mCamera->GetPosition());
			
			for (auto& trans : entity->GetComponentsOfType<Transform>())
			{
				shader->SetUniformMat4("model", trans->getComposed());
			}
		}
		for (auto& mesh : entity->GetComponentsOfType<Mesh>())
		{
			mesh->Draw();
		}
	}
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
