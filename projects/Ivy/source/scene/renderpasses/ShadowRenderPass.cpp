#include "ivypch.h"
#include "ShadowRenderPass.h"

void Ivy::ShadowRenderPass::Update()
{
	ImGui::Begin("Shadow Settings!");
	ImGui::Checkbox("Show Cascades", &mShowCascades);
	ImGui::Checkbox("Cascade fading", &mCascadeFading);
	ImGui::SliderFloat("Cascade Split Lambda", &mCascadeSplitLambda, 0.0f, 1.0f);
	ImGui::SliderFloat("Light Size", &mLightSize, 0.0f, 1.0f);
	ImGui::SliderFloat("Max shadow distance", &mMaxShadowDistance, 0.0f, 1000.0f);
	ImGui::SliderFloat("Shadow Fade", &mShadowFade, 0.0f, 250.0f);
	ImGui::SliderFloat("Far plane offset", &mCascadeFarPlaneOffset, 0.0f, 100.0f);
	ImGui::SliderFloat("Near plane offset", &mCascadeNearPlaneOffset, -100.0f, mCascadeFarPlaneOffset);
	ImGui::End();
}

void Ivy::ShadowRenderPass::RenderShadows(VecI2 windowSize, Vector<Ptr<Entity>>& entities)
{
	static Mat4 scaleBiasMatrix = glm::scale(Mat4(1.0f), { 0.5f, 0.5f, 0.5f }) * glm::translate(Mat4(1.0f), { 1, 1, 1 });

	CascadeData cascades[4];
	CalculateCascades(windowSize, cascades, mDirLight.direction);
	mLightViewMatrix = cascades[0].View;

	mDepthShader->Bind();


	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	for(int c = 0; c < CASCADE_COUNT; c++)
	{
		mCascadeSplits[c] = cascades[c].SplitDepth;
		BindFboForWriting(c);
		glClear(GL_DEPTH_BUFFER_BIT);
		mLightSpaceMatrices[c] = cascades[c].ViewProj;
		mDepthShader->SetUniformMat4("lightSpaceMatrix", mLightSpaceMatrices[c]);


		for(int i = 0; i < entities.size(); i++)
		{
			if(entities[i]->IsActive())
			{
				Ptr<Transform> trans = entities[i]->GetFirstComponentOfType<Transform>();
				Mat4 model = trans->getComposed();
				mDepthShader->SetUniformMat4("model", model);

				Vector<Ptr<Mesh>> meshes = entities[i]->GetComponentsOfType<Mesh>();
				for(int j = 0; j < meshes.size(); j++)
				{
					meshes[j]->Draw(mDepthShader, false);
				}
			}
		}

	}
	mShadowFBO->Unbind();
}

void Ivy::ShadowRenderPass::UpdateShaderUniforms(Ptr<Shader> shader)
{
	// Textures
	//glBindTextureUnit(8, mTextures[0]);
	//glBindTextureUnit(9, mTextures[1]);
	//glBindTextureUnit(10, mTextures[2]);
	//glBindTextureUnit(11, mTextures[3]);
	glBindTextures(8, mTextures.size(), mTextures.data());

	// Light Space Matrices
	Mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	Mat4 bias = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));


	
	shader->SetUniformMat4("lightSpaceMatrix0", bias * mLightSpaceMatrices[0]);
	shader->SetUniformMat4("lightSpaceMatrix1", bias * mLightSpaceMatrices[1]);
	shader->SetUniformMat4("lightSpaceMatrix2", bias * mLightSpaceMatrices[2]);
	shader->SetUniformMat4("lightSpaceMatrix3", bias * mLightSpaceMatrices[3]);

	//Params
	shader->SetUniformMat4("csm.lightView", mLightViewMatrix);	// TODO: weiß nicht ob das so stimmt??

	shader->SetUniformInt("csm.showCascades", mShowCascades);

	shader->SetUniformInt("csm.softShadows", mSoftShadows);

	shader->SetUniformFloat("csm.lightSize", mLightSize);

	shader->SetUniformFloat("csm.maxShadowDistance", mMaxShadowDistance);

	shader->SetUniformFloat("csm.shadowFade", mShadowFade);

	shader->SetUniformInt("csm.cascadeFading", mCascadeFading);

	shader->SetUniformFloat("csm.cascadeTransitionFade", mCascadeTransitionFade);

	shader->SetUniformFloat4("csm.cascadeSplits", mCascadeSplits);


}

void Ivy::ShadowRenderPass::SetupShader()
{
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

// Implementierung der Methode von Sasha Willems "Vulkan Examples" (https://github.com/SaschaWillems/Vulkan)
void Ivy::ShadowRenderPass::CalculateCascades(Vec2 currentWindowSize, CascadeData * cascades, const Vec3 & lightDirection)
{
	float cascadeSplits[4];

	float nearClip = 0.001f;
	float farClip = 512.0f;
	float clipRange = farClip - nearClip;

	float minZ = nearClip;
	float maxZ = nearClip + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	// Calculate split depths based on view camera furstum
	// Based on method presentd in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
	for(uint32_t i = 0; i < CASCADE_COUNT; i++)
	{
		float p = (i + 1) / static_cast<float>(CASCADE_COUNT);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = mCascadeSplitLambda * (log - uniform) + uniform;
		cascadeSplits[i] = (d - nearClip) / clipRange;
	}

	// Calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.0;
	for(uint32_t i = 0; i < CASCADE_COUNT; i++)
	{
		float splitDist = cascadeSplits[i];

		Vec3 frustumCorners[8] = {
			Vec3(-1.0f,  1.0f, -1.0f),
			Vec3(1.0f,  1.0f, -1.0f),
			Vec3(1.0f, -1.0f, -1.0f),
			Vec3(-1.0f, -1.0f, -1.0f),
			Vec3(-1.0f,  1.0f,  1.0f),
			Vec3(1.0f,  1.0f,  1.0f),
			Vec3(1.0f, -1.0f,  1.0f),
			Vec3(-1.0f, -1.0f,  1.0f),
		};

		// Project frustum corners into world space
		Mat4 proj = mCamera->GetProjectionMatrix(currentWindowSize);
		Mat4 view = mCamera->GetViewMatrix();
		Mat4 invCam = glm::inverse(proj * view);
		for(uint32_t i = 0; i < 8; i++)
		{
			glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
			frustumCorners[i] = invCorner / invCorner.w;
		}

		for(uint32_t i = 0; i < 4; i++)
		{
			Vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}

		// Get frustum center
		Vec3 frustumCenter = Vec3(0.0f);
		for(uint32_t i = 0; i < 8; i++)
		{
			frustumCenter += frustumCorners[i];
		}
		frustumCenter /= 8.0f;

		float radius = 0.0f;
		for(uint32_t i = 0; i < 8; i++)
		{
			float distance = glm::length(frustumCorners[i] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		Vec3 maxExtents = Vec3(radius);
		Vec3 minExtents = -maxExtents;

		Vec3 lightDir = normalize(-lightDirection);
		Mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, Vec3(0.0f, 1.0f, 0.0f));
		Mat4 lightOrthoMatrix = glm::ortho(
			minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f + mCascadeNearPlaneOffset, 
			maxExtents.z - minExtents.z + mCascadeFarPlaneOffset);
		
		// Store split distance and matrix in cascade
		cascades[i].SplitDepth = (0.1f + splitDist * clipRange) * -1.0f;
		cascades[i].ViewProj = lightOrthoMatrix * lightViewMatrix;

		lastSplitDist = cascadeSplits[i];
	}
}

void Ivy::ShadowRenderPass::CreateFramebuffer()
{
	// Create the FBO
	glGenFramebuffers(1, &mFBO);

	// Create the depth buffer
	glGenTextures(mTextures.size(), mTextures.data());

	for(uint32_t i = 0; i < mTextures.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, mTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTextures[0], 0);

	// Disable writes to the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		Debug::CoreError("FB error, status: {}", status);
	}
}

void Ivy::ShadowRenderPass::BindFboForWriting(uint32_t cascadeIndex)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTextures[cascadeIndex], 0);
}
