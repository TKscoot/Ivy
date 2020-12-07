#include "ivypch.h"
#include "PostprocessingRenderPass.h"

Ivy::PostprocessingRenderPass::PostprocessingRenderPass(Ptr<SceneRenderPass> scenePass)
	: mScenePass(scenePass)
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mShader->Bind();
	mVertexArray->Bind();

	glBindTextureUnit(0, mSceneColorTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	mVertexArray->Unbind();
}
