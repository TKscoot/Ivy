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

	mShader = CreatePtr<Shader>("shaders/Triangle.vert", "shaders/Triangle.frag");
}

void Ivy::Renderer::Render()
{
    float timeValue = glfwGetTime();

	static float lastFrame = 0.0f;
	float dt = timeValue - lastFrame;
	lastFrame = timeValue;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw our first mesh

    float colorValue = (sin(timeValue) / 2.0f) + 0.5f;

    //mTransform.setRotation(Vec3(0.0f, -timeValue * 5.0f, 0.0f));
	//mTransform.setPositionZ(-10.0f);

	Mat4 model = Mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

	mCamera->Update(dt);
	Mat4 view = mCamera->GetViewMatrix();

	//Mat4 view = Mat4(1.0f);
	//view = glm::lookAt(Vec3(0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f));
	
	Vec2 currentWindowSize = mWindow->GetWindowSize();
    Mat4 projection = glm::perspective(glm::radians(45.0f), currentWindowSize.x / currentWindowSize.y, 0.1f, 1000.0f);

    //mTexture->Bind(0);
    //mTexture1->Bind(1);
	//
	//// Bind shader to tell renderer to use it
	//mShader->Bind(); 
	//mShader->SetUniformMat4("view", view);
	//mShader->SetUniformMat4("projection", projection);

	// Draw our mesh    
    //mMesh->Draw();

	for(auto& entity : mScene->GetEntities())
	{
		for (auto& mat : entity->GetComponentsOfType<Material>())
		{
			for (auto& kv : mat->GetTextures())
			{
				kv.second->Bind(static_cast<uint32_t>(kv.first));
			}

			// TODO: Compare if this shader is the default shader
			mat->SetShader(mShader);
			mat->GetShader()->Bind();
			mat->GetShader()->SetUniformMat4("view", view);
			mat->GetShader()->SetUniformMat4("projection", projection);
			mat->GetShader()->SetUniformFloat3("viewPos", mCamera->GetPosition());
		}
		for (auto& trans : entity->GetComponentsOfType<Transform>())
		{
			mShader->SetUniformMat4("model", trans->getComposed());
		}
		for (auto& mesh : entity->GetComponentsOfType<Mesh>())
		{
			mesh->Draw();
		}
	}

    // mVertexArray->Unbind(); // no need to unbind it every time
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
