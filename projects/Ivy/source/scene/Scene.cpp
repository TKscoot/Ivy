#include "ivypch.h"
#include "Scene.h"

Ivy::Ptr<Ivy::Scene> Ivy::Scene::mInstance = nullptr;

Ivy::Scene::Scene()
{
	mCamera = CreatePtr<Camera>(Vec3(0.0f, 0.0f, 0.0f));

	SetupSkyboxShaders();
}

Ivy::Scene::~Scene()
{
}

Ivy::Ptr<Ivy::Entity> Ivy::Scene::CreateEntity()
{
	Ptr<Entity> ent = CreatePtr<Entity>(); 
	ent->mIndex = mEntities.size();
	ent->AddComponent(CreatePtr<Transform>());
	ent->AddComponent(CreatePtr<Material>());
	ent->OnCreate();
	Debug::CoreLog("Created entity with index {}", ent->mIndex);

	mEntities.push_back(ent);

	return ent; 
}

void Ivy::Scene::SetSkybox(Vector<String> filepaths)
{
	mSkyboxFilepaths = filepaths;
	SetupSkybox();
}

void Ivy::Scene::SetSkybox(String right, String left, String top, String bottom, String back, String front)
{
	Vector<String> filenames;
	filenames.push_back(right);
	filenames.push_back(left);
	filenames.push_back(top);
	filenames.push_back(bottom);
	filenames.push_back(back);
	filenames.push_back(front);

	SetSkybox(filenames);
}

void Ivy::Scene::Update()
{
	for (auto& e : mEntities)
	{
		e->OnUpdate();
		e->UpdateComponents();
	}
}

void Ivy::Scene::Render(float deltaTime, Vec2 currentWindowSize)
{
	mCamera->Update(deltaTime);
	Mat4 view = mCamera->GetViewMatrix();

	Mat4 projection = glm::perspective(glm::radians(45.0f), currentWindowSize.x / currentWindowSize.y, 0.1f, 1000.0f);

	for (int i = 0; i < mEntities.size(); i++)
	{
		Vector<Ptr<Material>> materials = mEntities[i]->GetComponentsOfType<Material>();
		Ptr<Shader> shader;
		for (int j = 0; j < materials.size(); j++)
		{
			//Debug::CoreLog("entidx: {}", materials[j]->GetEntityIndex());
			shader = materials[j]->GetShader();

			// Bind Textures to specific slot (diff, norm, ...)
			//for (auto& kv : materials[j]->GetTextures())
			//{
			//	kv.second->Bind(static_cast<int>(kv.first));
			//}

			// TODO: Compare if this shader is the default shader
			if (shader->GetRendererID() != Shader::GetCurrentlyUsedShaderID())
			{
				shader->Bind();
			}

		}

		shader->SetUniformMat4("view", view);
		shader->SetUniformMat4("projection", projection);
		shader->SetUniformFloat3("viewPos", mCamera->GetPosition());

		shader->SetUniformFloat3("lightPos", Vec3(5.0f, 3.0f, 0.0f));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));

		Ptr<Transform> trans = mEntities[i]->GetFirstComponentOfType<Transform>();
		shader->SetUniformMat4("model", trans->getComposed());

		Vector<Ptr<Mesh>> meshes = mEntities[i]->GetComponentsOfType<Mesh>();
		for (int j = 0; j < meshes.size(); j++)
		{
			meshes[j]->Draw();
		}
	}

	//Draw skybox
	if (mShouldRenderSkybox)
	{
		glDepthFunc(GL_LEQUAL);
		mSkyboxShader->Bind();
		view = Mat4(Mat3(mCamera->GetViewMatrix()));
		mSkyboxShader->SetUniformMat4("view", view);
		mSkyboxShader->SetUniformMat4("projection", projection);
		mSkyboxVertexArray->Bind();
		mSkyboxCubeTexture->Bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		mSkyboxVertexArray->Unbind();
		glDepthFunc(GL_LESS); // set depth function back to default
	}


}

void Ivy::Scene::SetupSkybox()
{
	if (mSkyboxFilepaths.empty())
	{
		Debug::Warning("Won't render skybox. No skybox textures set!");
		mShouldRenderSkybox = false;
		return;
	}
	mSkyboxCubeTexture = CreatePtr<TextureCube>(mSkyboxFilepaths);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	SetupSkyboxShaders();

	BufferLayout layout =
	{
		{ShaderDataType::Float3, "aPos", 0},

	};

	mSkyboxVertexArray = CreatePtr<VertexArray>(layout);
	mSkyboxVertexBuffer = CreatePtr<VertexBuffer>();
	mSkyboxVertexArray->SetVertexBuffer(mSkyboxVertexBuffer);
	mSkyboxVertexArray->Bind();
	mSkyboxVertexBuffer->SetBufferData(&skyboxVertices, sizeof(skyboxVertices));

	mSkyboxShader->Bind();
	mShouldRenderSkybox = true;
}

void Ivy::Scene::SetupSkyboxShaders()
{
	String vertexFilepath   = "shaders/Skybox.vert";
	String fragmentFilepath = "shaders/Skybox.frag";

	if (!std::filesystem::exists(vertexFilepath))
	{
		std::filesystem::path path{ vertexFilepath };
		std::filesystem::create_directories(path.parent_path());

		std::ofstream ofs(path);
		ofs << "#version 450 core\n";
		ofs << "layout(location = 0) in vec3 aPos;\n";
		ofs << "\n";
		ofs << "out vec3 TexCoords;\n";
		ofs << "\n";
		ofs << "uniform mat4 projection;\n";
		ofs << "uniform mat4 view;\n";
		ofs << "\n";
		ofs << "void main()\n";
		ofs << "{\n";
		ofs << "	TexCoords = aPos;\n";
		ofs << "	vec4 pos = projection * view * vec4(aPos, 1.0);\n";
		ofs << "	gl_Position = pos.xyww;\n";
		ofs << "}\n";
		ofs.close();
	}
	if (!std::filesystem::exists(fragmentFilepath))
	{
		std::filesystem::path path{ fragmentFilepath };
		std::filesystem::create_directories(path.parent_path());

		std::ofstream ofs(path);
		ofs << "#version 450 core\n";
		ofs << "out vec4 FragColor;\n";
		ofs << "\n";
		ofs << "in vec3 TexCoords;\n";
		ofs << "\n";
		ofs << "layout(binding = 0) uniform samplerCube skybox;\n";
		ofs << "\n";
		ofs << "void main()\n";
		ofs << "{\n";
		ofs << "	FragColor = texture(skybox, TexCoords);\n";
		ofs << "}\n";
		ofs.close();
	}

	mSkyboxShader = CreatePtr<Shader>(vertexFilepath, fragmentFilepath);
}
