#include "ivypch.h"
#include "Scene.h"

Ivy::Ptr<Ivy::Scene> Ivy::Scene::mInstance = nullptr;

Ivy::Scene::Scene()
{
	mCamera = CreatePtr<Camera>(Vec3(0.0f, 0.0f, 0.0f));

	SetupSkyboxShaders();
	SetupShadows();

	AddDirectionalLight(
		Vec3(-2.0f, 4.0f, -1.0f),		//direction
		Vec3(0.05f, 0.05f, 0.05f),		//ambient
		Vec3(0.8f, 0.8f, 0.8f),		//diffuse
		Vec3(0.5f, 0.5f, 0.5f));	//specular
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
	for(auto& e : mEntities)
	{
		e->OnUpdate();
		e->UpdateComponents();
	}
}

void Ivy::Scene::Render(float deltaTime, Vec2 currentWindowSize)
{
	// Shadow pass

	//mShadowFBO->Bind();
	//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//glCullFace(GL_FRONT);
	//
	//Mat4 lightProjection, lightView, lightSpaceMatrix;
	//float nearPlane = 0.01f, farPlane = 96.0f;
	//lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
	////lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);
	//lightView = glm::lookAt(mDirLight.direction, Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));
	//
	//Mat4 biasMatrix(
	//	0.5, 0.0, 0.0, 0.0,
	//	0.0, 0.5, 0.0, 0.0,
	//	0.0, 0.0, 0.5, 0.0,
	//	0.5, 0.5, 0.5, 1.0
	//);
	//
	//lightSpaceMatrix = lightProjection * lightView;
	////lightSpaceMatrix = biasMatrix * lightSpaceMatrix;
	//
	//mDepthShader->Bind();
	//mDepthShader->SetUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
	//
	//
	//for(int i = 0; i < mEntities.size(); i++)
	//{
	//	Ptr<Transform> trans = mEntities[i]->GetFirstComponentOfType<Transform>();
	//	mDepthShader->SetUniformMat4("model", trans->getComposed());
	//
	//	Vector<Ptr<Mesh>> meshes = mEntities[i]->GetComponentsOfType<Mesh>();
	//	for(int j = 0; j < meshes.size(); j++)
	//	{
	//		meshes[j]->Draw(false);
	//	}
	//}
	//
	//mShadowFBO->Unbind();
	//
	//glCullFace(GL_BACK);

	RenderShadows();

	// Scene pass


	glViewport(0, 0, currentWindowSize.x, currentWindowSize.y);

	mCamera->Update(deltaTime);
	Mat4 view = mCamera->GetViewMatrix();

	Mat4 projection = glm::perspective(glm::radians(45.0f), currentWindowSize.x / currentWindowSize.y, 0.1f, 100000.0f);

	for(int i = 0; i < mEntities.size(); i++)
	{
		Vector<Ptr<Material>> materials = mEntities[i]->GetComponentsOfType<Material>();
		Ptr<Shader> shader;
		for(int j = 0; j < materials.size(); j++)
		{
			//Debug::CoreLog("entidx: {}", materials[j]->GetEntityIndex());
			shader = materials[j]->GetShader();

			// Bind Textures to specific slot (diff, norm, ...)
			//for (auto& kv : materials[j]->GetTextures())
			//{
			//	kv.second->Bind(static_cast<int>(kv.first));
			//}

			// TODO: Compare if this shader is the default shader
			if(shader->GetRendererID() != Shader::GetCurrentlyUsedShaderID())
			{
				shader->Bind();
			}

		}

		shader->SetUniformMat4("view", view);
		shader->SetUniformMat4("projection", projection);

		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);
		glm::mat4 depthBiasMVP = biasMatrix * lightSpaceMatrix;
		shader->SetUniformMat4("lightSpaceMatrix", depthBiasMVP);

		shader->SetUniformFloat3("viewPos", mCamera->GetPosition());

		shader->SetUniformFloat3("lightPos", Vec3(5.0f, 3.0f, 0.0f));
		shader->SetUniformFloat3("sunDirection", Vec3(-0.2f, -1.0f, -0.3f));
		shader->SetUniformFloat3("sunColor", Vec3(252.0F, 212.0f, 64.0f));

		glBindTextureUnit(8, mShadowFBO->GetDepthTextureID());

		PushLightParams(shader);

		Ptr<Transform> trans = mEntities[i]->GetFirstComponentOfType<Transform>();
		shader->SetUniformMat4("model", trans->getComposed());

		Vector<Ptr<Mesh>> meshes = mEntities[i]->GetComponentsOfType<Mesh>();
		for(int j = 0; j < meshes.size(); j++)
		{
			meshes[j]->Draw();
		}
	}

	//Draw skybox
	if(mShouldRenderSkybox)
	{
		glDepthFunc(GL_LEQUAL);
		mSkyboxShader->Bind();
		view = Mat4(Mat3(mCamera->GetViewMatrix()));
		mSkyboxShader->SetUniformMat4("view", view);
		mSkyboxShader->SetUniformMat4("projection", projection);
		mSkyboxShader->SetUniformFloat3("sunPosition", mDirLight.direction);
		mSkyboxVertexArray->Bind();
		mSkyboxCubeTexture->Bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		mSkyboxVertexArray->Unbind();
		glDepthFunc(GL_LESS); // set depth function back to default
	}


}



void Ivy::Scene::SetupSkybox()
{
	if(mSkyboxFilepaths.empty())
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
	String vertexFilepath = "shaders/Skybox.vert";
	String fragmentFilepath = "shaders/Skybox.frag";

	if(!std::filesystem::exists(vertexFilepath))
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
	if(!std::filesystem::exists(fragmentFilepath))
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

void Ivy::Scene::PushLightParams(Ptr<Shader> shader)
{
	if(!shader)
	{
		Debug::CoreError("Light can't be setup. Shader is null!");
	}
	if(shader->GetRendererID() != Shader::GetCurrentlyUsedShaderID())
	{
		shader->Bind();
	}

	// Directional Light
	shader->SetUniformFloat3("dirLight.direction", mDirLight.direction);
	shader->SetUniformFloat3("dirLight.ambient", mDirLight.ambient);
	shader->SetUniformFloat3("dirLight.diffuse", mDirLight.diffuse);
	shader->SetUniformFloat3("dirLight.specular", mDirLight.specular);

	// Point Lights
	shader->SetUniformInt("pointLightSize", mPointLights.size());
	for(int i = 0; i < mPointLights.size(); i++)
	{
		String index = std::to_string(i);
		shader->SetUniformFloat3("pointLights[" + index + "].position", mPointLights[i].position);
		shader->SetUniformFloat3("pointLights[" + index + "].ambient", mPointLights[i].ambient);
		shader->SetUniformFloat3("pointLights[" + index + "].diffuse", mPointLights[i].diffuse);
		shader->SetUniformFloat3("pointLights[" + index + "].specular", mPointLights[i].specular);
		shader->SetUniformFloat("pointLights[" + index + "].constant", mPointLights[i].constant);
		shader->SetUniformFloat("pointLights[" + index + "].linear", mPointLights[i].linear);
		shader->SetUniformFloat("pointLights[" + index + "].quadratic", mPointLights[i].quadratic);
	}

	// Spot Lights
	shader->SetUniformInt("spotLightSize", mSpotLights.size());
	for(int i = 0; i < mSpotLights.size(); i++)
	{
		String index = std::to_string(i);
		shader->SetUniformFloat3("spotLights[" + index + "].position", mSpotLights[i].position);
		shader->SetUniformFloat3("spotLights[" + index + "].ambient", mSpotLights[i].ambient);
		shader->SetUniformFloat3("spotLights[" + index + "].diffuse", mSpotLights[i].diffuse);
		shader->SetUniformFloat3("spotLights[" + index + "].specular", mSpotLights[i].specular);
		shader->SetUniformFloat("spotLights[" + index + "].constant", mSpotLights[i].constant);
		shader->SetUniformFloat("spotLights[" + index + "].linear", mSpotLights[i].linear);
		shader->SetUniformFloat("spotLights[" + index + "].quadratic", mSpotLights[i].quadratic);
		shader->SetUniformFloat("spotLights[" + index + "].cutOff", glm::cos(glm::radians(mSpotLights[i].cutOff)));
		shader->SetUniformFloat("spotLights[" + index + "].outerCutOff", glm::cos(glm::radians(mSpotLights[i].outerCutOff)));
	}
}

void Ivy::Scene::RenderEntities()
{
}

void Ivy::Scene::SetupShadows()
{
	mShadowFBO = CreatePtr<Framebuffer>(SHADOW_WIDTH, SHADOW_HEIGHT, 1);
	//mShadowFBO->Prepare();

	String vertexFilepath = "shaders/Shadow.vert";
	String fragmentFilepath = "shaders/Shadow.frag";

	if(!std::filesystem::exists(vertexFilepath))
	{
		std::filesystem::path path{ vertexFilepath };
		std::filesystem::create_directories(path.parent_path());

		std::ofstream ofs(path);
		ofs << "#version 330 core\n";
		ofs << "layout(location = 0) in vec3 aPos;\n";
		ofs << "\n";
		ofs << "uniform mat4 lightSpaceMatrix;\n";
		ofs << "uniform mat4 model;\n";
		ofs << "\n";
		ofs << "void main()\n";
		ofs << "{\n";
		ofs << "	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);\n";
		ofs << "}\n";
		ofs.close();
	}
	if(!std::filesystem::exists(fragmentFilepath))
	{
		std::filesystem::path path{ fragmentFilepath };
		std::filesystem::create_directories(path.parent_path());

		std::ofstream ofs(path);
		ofs << "#version 330 core\n";
		ofs << "\n";
		ofs << "void main()\n";
		ofs << "{\n";
		ofs << "	// gl_FragDepth = gl_FragCoord.z;\n";
		ofs << "}\n";
		ofs.close();
	}

	mDepthShader = CreatePtr<Shader>(vertexFilepath, fragmentFilepath);
}

void Ivy::Scene::RenderShadows()
{
	Mat4 lightProjection, lightView, lightModel;
	
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
	//lightProjection = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 25.0f);
	lightView = glm::lookAt(mDirLight.direction, Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));
	Mat4 bias = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));


	
	lightSpaceMatrix = lightProjection * lightView ;
	
	mDepthShader->Bind();
	mDepthShader->SetUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
	
	
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, mShadowFBO->GetDepthFboID());
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mShadowFBO->GetDepthTextureID());
	
	for(int i = 0; i < mEntities.size(); i++)
	{
		Ptr<Transform> trans = mEntities[i]->GetFirstComponentOfType<Transform>();
		mDepthShader->SetUniformMat4("model", trans->getComposed());
	
		Vector<Ptr<Mesh>> meshes = mEntities[i]->GetComponentsOfType<Mesh>();
		for(int j = 0; j < meshes.size(); j++)
		{
			meshes[j]->Draw(false);
		}
	}
	
	mShadowFBO->Unbind();
	//glCullFace(GL_BACK);
}

Ivy::DirectionalLight& Ivy::Scene::AddDirectionalLight(DirectionalLight lightParams)
{
	mDirLight = lightParams;
	return mDirLight;
}

Ivy::DirectionalLight& Ivy::Scene::AddDirectionalLight(
	Vec3 direction, Vec3 ambient, Vec3 diffuse, Vec3 specular)
{
	DirectionalLight l;
	l.direction = direction;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;

	mDirLight = l;

	return mDirLight;
}

Ivy::PointLight& Ivy::Scene::AddPointLight(PointLight lightParams)
{
	mPointLights.push_back(lightParams);

	return lightParams;
}

Ivy::PointLight& Ivy::Scene::AddPointLight(
	Vec3 position, float constant, float linear,
	float quadratic, Vec3 ambient, Vec3 diffuse, Vec3 specular)
{
	PointLight l;
	l.position = position;
	l.constant = constant;
	l.linear = linear;
	l.quadratic = quadratic;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;

	mPointLights.push_back(l);

	return l;
}

Ivy::SpotLight& Ivy::Scene::AddSpotLight(SpotLight& lightParams)
{
	mSpotLights.push_back(lightParams);

	return lightParams;
}

Ivy::SpotLight& Ivy::Scene::AddSpotLight(
	Vec3 position, Vec3 direction, float cutOff,
	float outerCutOff, float constant, float linear,
	float quadratic, Vec3 ambient, Vec3 diffuse, Vec3 specular)
{
	SpotLight l;
	l.position = position;
	l.direction = direction;
	l.cutOff = cutOff;
	l.outerCutOff = outerCutOff;
	l.constant = constant;
	l.linear = linear;
	l.quadratic = quadratic;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;

	mSpotLights.push_back(l);

	return l;
}
