#include "ivypch.h"
#include "SceneRenderPass.h"

Ivy::SceneRenderPass::SceneRenderPass(
	Ptr<Camera> camera, 
	Ptr<Window>				window,
	Vector<Ptr<Entity>>&	entities,
	Ptr<ShadowRenderPass>   shadowPass,
	Ptr<SkyModel>			skyModel,
	DirectionalLight&		dirLight,
	Vector<SpotLight>&		spotLights,
	Vector<PointLight>&		pointLights)
	: mCamera(camera)
	, mWindow(window)
	, mEntities(entities)
	, mCSM(shadowPass)
	, mSkyModel(skyModel)
	, mDirLight(dirLight)
	, mSpotLights(spotLights)
	, mPointLights(pointLights)
{
	mWindowSize = mWindow->GetWindowSize();

	//GLubyte* pixelData = new GLubyte[2 * 2];

	//std::array<GLubyte, 4> pixelData;


	Vector<String> filepaths;
	for(int i = 0; i < 6; i++)
	{
		filepaths.push_back("assets/textures/Misc/Grey.png");
	}

	mEnvUnfiltered = CreatePtr<TextureCube>(filepaths);

	SetupSkybox(mEnvUnfiltered);

	SetupFramebuffer();
}

void Ivy::SceneRenderPass::Render(float deltaTime, Vec2 currentWindowSize, bool renderHosekSky)
{
	BindFramebufferForWrite();



	if(mWindowSize != (VecI2)currentWindowSize)
	{
		mWindowSize = currentWindowSize;
		// TODO: Framebuffer resize
		glBindTexture(GL_TEXTURE_2D, mDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mWindowSize.x, mWindowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, mColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWindowSize.x, mWindowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glBindTexture(GL_TEXTURE_2D, mGodrayOcclusionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWindowSize.x, mWindowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, mWindowSize.x, mWindowSize.y);
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, currentWindowSize.x, currentWindowSize.y);

	mRenderData.previousProj = mRenderData.proj;
	mRenderData.previousView = mRenderData.view;
	mRenderData.previousViewProj = mRenderData.view * mRenderData.proj;
	mRenderData.previousCameraPosition = mRenderData.cameraPosition;

	Mat4 view = mCamera->GetViewMatrix();
	Mat4 projection = mCamera->GetProjectionMatrix(currentWindowSize);

	mRenderData.view			 = view;
	mRenderData.proj			 = projection;
	Mat4 viewProj				 = view * projection;
	mRenderData.invViewProj		 = glm::inverse(viewProj);
	mRenderData.windowResolution = mWindowSize;

	mRenderData.cameraPosition = mCamera->GetPosition();

	mRenderData.nearPlane = mCamera->GetNearPlane();
	mRenderData.farPlane = mCamera->GetFarPlane();


	//Draw skybox
	if(mShouldRenderSkybox)
	{

		glDepthMask(GL_FALSE);



		mSkyboxShader->Bind();
		mSkyboxShader->SetUniformMat4("view", glm::mat4(glm::mat3(mCamera->GetViewMatrix())));
		mSkyboxShader->SetUniformMat4("projection", projection);
		mSkyboxShader->SetUniformFloat3("sunPosition", mDirLight.direction);

		if (renderHosekSky)
		{
			mCloudsData.time += deltaTime;
			
			mSkyModel->SetDirection(mDirLight.direction);
			mSkyModel->Update();
			mSkyboxShader->SetUniformInt("calculateClouds", 1);
			mSkyboxShader->SetUniformFloat("time", mCloudsData.time);
			mSkyboxShader->SetUniformFloat("cirrus", mCloudsData.cirrus);
			mSkyboxShader->SetUniformFloat("cumulus", mCloudsData.cumulus);
		}
		else
		{
			mSkyboxShader->SetUniformInt("calculateClouds", 0);
		}

		mSkyModel->SetRenderUniforms(mSkyboxShader);

		mSkyboxVertexArray->Bind();

		mEnvUnfiltered->Bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		mSkyboxVertexArray->Unbind();

		glDepthMask(GL_TRUE);
	}



	for(int i = 0; i < mEntities.size(); i++)
	{
		if(!mEntities[i]->IsActive())
		{
			continue;
		}

		mEntities[i]->OnDraw(mCamera, currentWindowSize);

		Vector<Ptr<Material>> materials = mEntities[i]->GetComponentsOfType<Material>();
		if (materials.empty())
		{
			continue;
		}

		Ptr<Shader> shader;
		for(int j = 0; j < materials.size(); j++)
		{
			shader = materials[j]->GetShader();

			if(shader->GetRendererID() != Shader::GetCurrentlyUsedShaderID())
			{
				shader->Bind();
			}
		}

		shader->SetUniformMat4("view", view);
		shader->SetUniformMat4("projection", projection);

		// Updating cascaded shadow uniforms
		mCSM->UpdateShaderUniforms(shader);

		shader->SetUniformFloat3("viewPos", mCamera->GetPosition());

		if(mEnvUnfiltered)
		{
			mEnvUnfiltered->Bind(6);
			if(mUseHdri)
			{
				mIrradiance->Bind(7);
			}
		}

		PushLightParams(shader);

		Vector<Ptr<Mesh>> meshes = mEntities[i]->GetComponentsOfType<Mesh>();
		for(int j = 0; j < meshes.size(); j++)
		{
			meshes[j]->Draw();
			if (meshes[j]->mDrawBoundingBox)
			{
				meshes[j]->DrawBoundingBox(projection, view);
			}
		}

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Ivy::SceneRenderPass::UnloadEnvironmentMap()
{
	if (mEquirectangularConversionShader)
	{
		mEquirectangularConversionShader->Destroy();
	}

	if (mEnvFilteringShader)
	{
		mEnvFilteringShader->Destroy();
	}
	
	if (mIrradianceShader)
	{
		mIrradianceShader->Destroy();
	}

	if (mEnvUnfiltered)
	{
		mEnvUnfiltered->Destroy();
	}

	if (mEnvEquirect)
	{
		mEnvEquirect->Destroy();
	}
	
	if (mEnvFiltered)
	{
		mEnvFiltered->Destroy();
	}

	if (mIrradiance)
	{
		mIrradiance->Destroy();
	}
}

void Ivy::SceneRenderPass::DestroySkybox()
{

	mSkyboxShader->Destroy();
	mSkyboxVertexBuffer->Destroy();
	mSkyboxVertexArray->Destroy();

	mShouldRenderSkybox = false;
}

void Ivy::SceneRenderPass::PushLightParams(Ptr<Shader> shader)
{
	if(!shader)
	{
		Debug::CoreError("Lights can't be setup. Shader is null!");
	}
	if(shader->GetRendererID() != Shader::GetCurrentlyUsedShaderID())
	{
		shader->Bind();
	}

	// Directional Light
	shader->SetUniformFloat( "dirLight.intensity", mDirLight.intensity);
	shader->SetUniformFloat3("dirLight.direction", mDirLight.direction);

	// Point Lights
	shader->SetUniformInt("pointLightSize", mPointLights.size());
	for(int i = 0; i < mPointLights.size(); i++)
	{
		String index = std::to_string(i);
		shader->SetUniformFloat3("pointLights[" + index + "].position",  mPointLights[i].position);
		shader->SetUniformFloat3("pointLights[" + index + "].ambient",	 mPointLights[i].ambient);
		shader->SetUniformFloat3("pointLights[" + index + "].diffuse",	 mPointLights[i].diffuse);
		shader->SetUniformFloat3("pointLights[" + index + "].specular",  mPointLights[i].specular);
		shader->SetUniformFloat( "pointLights[" + index + "].constant",  mPointLights[i].constant);
		shader->SetUniformFloat( "pointLights[" + index + "].linear",	 mPointLights[i].linear);
		shader->SetUniformFloat( "pointLights[" + index + "].quadratic", mPointLights[i].quadratic);
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
		shader->SetUniformFloat( "spotLights[" + index + "].constant", mSpotLights[i].constant);
		shader->SetUniformFloat( "spotLights[" + index + "].linear", mSpotLights[i].linear);
		shader->SetUniformFloat( "spotLights[" + index + "].quadratic", mSpotLights[i].quadratic);
		shader->SetUniformFloat( "spotLights[" + index + "].cutOff", glm::cos(glm::radians(mSpotLights[i].cutOff)));
		shader->SetUniformFloat( "spotLights[" + index + "].outerCutOff", glm::cos(glm::radians(mSpotLights[i].outerCutOff)));
	}
}


void Ivy::SceneRenderPass::SetupSkybox(Vector<String> filepaths)
{
	mSkyboxFilepaths = filepaths;
	if(mSkyboxFilepaths.empty())
	{
		Debug::Warning("Won't render skybox. No skybox textures set!");
		mShouldRenderSkybox = false;
		return;
	}
	mEnvUnfiltered = CreatePtr<TextureCube>(mSkyboxFilepaths);

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
		{ShaderDataType::Float3, "aPos", 0}
	};

	mSkyboxVertexArray = CreatePtr<VertexArray>(layout);
	mSkyboxVertexBuffer = CreatePtr<VertexBuffer>();
	mSkyboxVertexArray->SetVertexBuffer(mSkyboxVertexBuffer);
	mSkyboxVertexArray->Bind();
	mSkyboxVertexBuffer->SetBufferData(&skyboxVertices, sizeof(skyboxVertices));

	mSkyboxShader->Bind();
	mShouldRenderSkybox = true;
}

void Ivy::SceneRenderPass::SetupSkybox(Ptr<TextureCube> tex)
{
	mEnvUnfiltered = tex;

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
		{ShaderDataType::Float3, "aPos", 0}
	};

	mSkyboxVertexArray = CreatePtr<VertexArray>(layout);
	mSkyboxVertexBuffer = CreatePtr<VertexBuffer>();
	mSkyboxVertexArray->SetVertexBuffer(mSkyboxVertexBuffer);
	mSkyboxVertexArray->Bind();
	mSkyboxVertexBuffer->SetBufferData(&skyboxVertices, sizeof(skyboxVertices));

	mSkyboxShader->Bind();
	mShouldRenderSkybox = true;
}

void Ivy::SceneRenderPass::SetupSkyboxShaders()
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



void Ivy::SceneRenderPass::SetupFramebuffer()
{
	// Create the FBO
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	glCreateTextures(GL_TEXTURE_2D, 1, &mDepthTexture);
	glCreateTextures(GL_TEXTURE_2D, 1, &mColorTexture);
	glCreateTextures(GL_TEXTURE_2D, 1, &mGodrayOcclusionTexture);

	glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mWindowSize.x, mWindowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, mColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWindowSize.x, mWindowSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, mGodrayOcclusionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWindowSize.x, mWindowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Depth buffer 
	glGenRenderbuffers(1, &mRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWindowSize.x, mWindowSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGodrayOcclusionTexture, 0);
	// Set the list of draw buffers.
	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers); 


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		Debug::CoreError("FB error, status: {}", status);
	}
}

void Ivy::SceneRenderPass::BindFramebufferForWrite()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

void Ivy::SceneRenderPass::SetEnvironmentMap(String filepath)
{
	const uint32_t cubemapSize = 2048;

	mEnvUnfiltered = CreatePtr<TextureCube>(GL_RGBA16F, cubemapSize, cubemapSize);


	mEnvEquirect = CreatePtr<TextureHDRI>(filepath);
	
	mEnvEquirect->ConvertToCubemap(mEnvUnfiltered);
	
	ComputeEnvironmentMap();

	SetupSkybox(mEnvUnfiltered);

	mUseHdri = true;
}

void Ivy::SceneRenderPass::SetEnvironmentMap(Ptr<TextureCube> cubemap)
{
	if (!cubemap)
	{
		Debug::CoreError("Invalid cubemap for environment map!");
		return;
	}

	mEnvUnfiltered = cubemap;

	ComputeEnvironmentMap();

	SetupSkybox(mEnvUnfiltered);

	mUseHdri = true;
}

void Ivy::SceneRenderPass::SetupSkyModel()
{
	RenderSkyModelToCubemap();

	ComputeEnvironmentMap();

	SetupSkybox(mEnvUnfiltered);

	mUseHdri = true;
}

void Ivy::SceneRenderPass::ComputeEnvironmentMap()
{
	if (!mEnvUnfiltered || mEnvUnfiltered->GetFormat() != GL_RGBA16F)
	{
		mEnvUnfiltered = CreatePtr<TextureCube>(GL_RGBA16F, 2048, 2048);
	}

	const uint32_t irradianceMapSize = 32;

	mEnvFilteringShader = CreatePtr<Shader>("shaders/EnvironmentMipFilter.comp");

	mEnvFiltered = CreatePtr<TextureCube>(GL_RGBA16F, mEnvUnfiltered->GetWidth(), mEnvUnfiltered->GetHeight());

	glCopyImageSubData(mEnvUnfiltered->GetID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
		mEnvFiltered->GetID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
		mEnvFiltered->GetWidth(), mEnvFiltered->GetHeight(), 6);

	mEnvFilteringShader->Bind();
	mEnvUnfiltered->Bind();

	const float deltaRoughness = 1.0f / glm::max((float)(mEnvFiltered->GetMipLevelCount() - 1.0f), 1.0f);
	for (int level = 1, size = mEnvUnfiltered->GetWidth() / 2; level < mEnvFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
	{
		const GLuint numGroups = glm::max(1, size / 32);
		glBindImageTexture(0, mEnvFiltered->GetID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glProgramUniform1f(mEnvFilteringShader->GetRendererID(), 0, level * deltaRoughness);
		glDispatchCompute(numGroups, numGroups, 6);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	mIrradianceShader = CreatePtr<Shader>("shaders/EnvironmentIrradiance.comp");
	mIrradiance = CreatePtr<TextureCube>(GL_RGBA16F, irradianceMapSize, irradianceMapSize);
	mIrradianceShader->Bind();
	mEnvFiltered->Bind();

	glBindImageTexture(0, mIrradiance->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(mIrradiance->GetWidth() / 32, mIrradiance->GetHeight() / 32, 6);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glGenerateTextureMipmap(mIrradiance->GetID());

	mEnvFiltered->Destroy();
}

void Ivy::SceneRenderPass::RenderSkyModelToCubemap()
{
	Ptr<Shader> skyToCubemapShader = CreatePtr<Shader>("shaders/HosekWilkieSkyModel.comp");

	const uint32_t cubemapSize = 2048;

	if (!mEnvUnfiltered)
	{
		mEnvUnfiltered = CreatePtr<TextureCube>(GL_RGBA16F, cubemapSize, cubemapSize);
	}

	skyToCubemapShader->Bind();

	mSkyModel->Update();
	mSkyModel->SetRenderUniforms(skyToCubemapShader);

	glBindImageTexture(0, mEnvUnfiltered->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(mEnvUnfiltered->GetWidth() / 32, mEnvUnfiltered->GetHeight() / 32, 6);

	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glGenerateTextureMipmap(mEnvUnfiltered->GetID());
}
