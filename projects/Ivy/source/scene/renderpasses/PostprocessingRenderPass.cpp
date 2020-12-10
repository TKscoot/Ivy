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

}

void Ivy::PostprocessingRenderPass::Render(Vec2 currentWindowSize)
{
	ImGui::Begin("Postprocessing Settings!");
	ImGui::SliderInt("Tonemap index", &mToneMapIndex, 0, 5);
	ImGui::End();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mShader->Bind();
	mVertexArray->Bind();

	UploadUniforms();

	glBindTextureUnit(0, mSceneColorTexture);
	glBindTextureUnit(1, mSceneDepthTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	mVertexArray->Unbind();
}

void Ivy::PostprocessingRenderPass::UploadUniforms()
{
	if(!mShader)
	{
		Debug::CoreError("Postrprocessing shader not bound or null");
	}
	mSceneData = mScenePass->GetSceneRenderData();

	mShader->SetUniformMat4(  "World",						mSceneData.world);
	mShader->SetUniformMat4(  "View",							mSceneData.view);
	mShader->SetUniformMat4(  "Projection",					mSceneData.proj);
	mShader->SetUniformMat4(  "WorldViewProjection",			mSceneData.wvp);
	mShader->SetUniformMat4(  "PreviousWorldViewProjection",  mSceneData.previousViewProj);
	mShader->SetUniformMat4(  "InverseViewProjection",		mSceneData.invViewProj);
	mShader->SetUniformFloat2("WindowResolution",			mSceneData.windowResolution);
	mShader->SetUniformInt(	  "Tonemap",						mToneMapIndex);

}
