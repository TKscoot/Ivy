#include "ivypch.h"
#include "SceneRenderPass.h"

Ivy::SceneRenderPass::SceneRenderPass(Ptr<Camera> camera, 
	Ptr<Window>				window,
	Vector<Ptr<Entity>>&	entities,
	Ptr<ShadowRenderPass>   shadowPass,
	DirectionalLight&		dirLight,
	Vector<SpotLight>&		spotLights,
	Vector<PointLight>&		pointLights)
	: mCamera(camera)
	, mWindow(window)
	, mEntities(entities)
	, mCSM(shadowPass)
	, mDirLight(dirLight)
	, mSpotLights(spotLights)
	, mPointLights(pointLights)
{
	mWindowSize = mWindow->GetWindowSize();
	
	SetupSkyboxShaders();

	SetupFramebuffer();

}

void Ivy::SceneRenderPass::Render(Vec2 currentWindowSize)
{
	if(mWindowSize != (VecI2)currentWindowSize)
	{
		mWindowSize = currentWindowSize;
		// TODO: Framebuffer resize
		glBindTexture(GL_TEXTURE_2D, mColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWindowSize.x, mWindowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWindowSize.x, mWindowSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	BindFramebufferForWrite();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, currentWindowSize.x, currentWindowSize.y);

	Mat4 view = mCamera->GetViewMatrix();
	Mat4 projection = mCamera->GetProjectionMatrix(currentWindowSize);

	for(int i = 0; i < mEntities.size(); i++)
	{
		Vector<Ptr<Material>> materials = mEntities[i]->GetComponentsOfType<Material>();
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

		shader->SetUniformFloat3("lightPos", Vec3(5.0f, 3.0f, 0.0f));
		shader->SetUniformFloat3("sunDirection", Vec3(-0.2f, -1.0f, -0.3f));
		shader->SetUniformFloat3("sunColor", Vec3(252.0F, 212.0f, 64.0f));

		mSkyboxCubeTexture->Bind(6);

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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


void Ivy::SceneRenderPass::SetupSkybox(Vector<String> filepaths)
{
	mSkyboxFilepaths = filepaths;
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

	// Create the depth buffer
	//glGenTextures(mTextures.size(), mTextures.data());
	glCreateTextures(GL_TEXTURE_2D, 1, &mDepthTexture);
	glCreateTextures(GL_TEXTURE_2D, 1, &mColorTexture);

	glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mWindowSize.x, mWindowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, mColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWindowSize.x, mWindowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Depth buffer */
	glGenRenderbuffers(1, &mRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWindowSize.x, mWindowSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO);
	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		Debug::CoreError("FB error, status: {}", status);
	}
}

void Ivy::SceneRenderPass::BindFramebufferForWrite()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);
}

