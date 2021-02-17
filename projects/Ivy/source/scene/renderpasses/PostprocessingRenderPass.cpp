#include "ivypch.h"
#include "PostprocessingRenderPass.h"

Ivy::PostprocessingRenderPass::PostprocessingRenderPass(Ptr<SceneRenderPass> scenePass)
	: mScenePass(scenePass)
	, mSceneData(mScenePass->GetSceneRenderData())
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

	mShader = CreatePtr<Shader>("shaders/Postprocessing.vert", "shaders/Postprocessing.frag");

	mVertexArray = CreatePtr<VertexArray>(layout);
	mVertexBuffer = CreatePtr<VertexBuffer>();
	mVertexArray->SetVertexBuffer(mVertexBuffer);
	mVertexArray->Bind();
	mVertexBuffer->SetBufferData(&vertices, sizeof(vertices));

	mSceneColorTexture = mScenePass->GetColorTextureID();
	mSceneDepthTexture = mScenePass->GetDepthTextureID();

}

void Ivy::PostprocessingRenderPass::Render(Vec2 currentWindowSize, float deltaTime)
{
	if(Input::IsKeyDown(KeyCode::P))
	{
		mUseMotionBlur = !mUseMotionBlur;
	}
	if(Input::IsKeyDown(KeyCode::L))
	{
		mUseDoF = !mUseDoF;
	}

	ImGui::Begin("Postprocessing Settings!");
	ImGui::Checkbox("Use depth of field", &mUseDoF);
	ImGui::SliderFloat("DoF Threshold", &mDofThreshold, 0, 1);
	ImGui::Checkbox("Use motion blur", &mUseMotionBlur);
	ImGui::SliderFloat("Motion blur intensity", &mMotionBlurIntensity, 1.0f, 30.0f);
	ImGui::SliderInt("Tonemap index", &mToneMapIndex, 0, 4);
	ImGui::End();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mShader->Bind();
	mVertexArray->Bind();

	UploadUniforms(deltaTime);

	glBindTextureUnit(0, mSceneColorTexture);
	glBindTextureUnit(1, mSceneDepthTexture);
	glBindTextureUnit(2, mScenePass->GetGodrayOcclusionTextureID());

	glDrawArrays(GL_TRIANGLES, 0, 36);

	mVertexArray->Unbind();
}

void Ivy::PostprocessingRenderPass::UploadUniforms(float deltaTime)
{
	if(!mShader)
	{
		Debug::CoreError("Postrprocessing shader not bound or null");
	}
	mSceneData = mScenePass->GetSceneRenderData();

	mShader->SetUniformMat4(  "World",						mSceneData.world);
	mShader->SetUniformMat4(  "View",						mSceneData.view);
	mShader->SetUniformMat4(  "Projection",					mSceneData.proj);
	mShader->SetUniformMat4(  "WorldViewProjection",		mSceneData.wvp);
	mShader->SetUniformMat4(  "PreviousView",				mSceneData.previousView);
	mShader->SetUniformMat4(  "PreviousProjection",			mSceneData.previousProj);
	mShader->SetUniformMat4(  "PreviousViewProjection",		mSceneData.previousViewProj);
	mShader->SetUniformMat4(  "InverseViewProjection",		mSceneData.invViewProj);
	mShader->SetUniformFloat2("WindowResolution",			mSceneData.windowResolution);
	mShader->SetUniformFloat3("CameraPositon",				mSceneData.cameraPosition);
	mShader->SetUniformFloat3("PreviousCameraPositon",		mSceneData.previousCameraPosition);
	mShader->SetUniformFloat( "NearPlane",					mSceneData.nearPlane);
	mShader->SetUniformFloat( "FarPlane",					mSceneData.farPlane);
	mShader->SetUniformFloat( "FrameTime",					deltaTime);
	mShader->SetUniformInt(	  "Tonemap",					mToneMapIndex);
	mShader->SetUniformInt(	  "UseDepthOfField",	        mUseDoF);
	mShader->SetUniformInt(	  "UseMotionBlur",				mUseMotionBlur);
	mShader->SetUniformFloat("MotionBlurIntensity",			mMotionBlurIntensity);
	mShader->SetUniformFloat("DofThreshold",				mDofThreshold);

}
