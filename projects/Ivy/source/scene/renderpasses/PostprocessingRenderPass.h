#pragma once
#include "Types.h"
#include "rendering/Shader.h"
#include "rendering/Buffer.h"
#include "rendering/BufferLayout.h"
#include "rendering/VertexArray.h"
#include "SceneRenderPass.h"

namespace Ivy
{
	class PostprocessingRenderPass
	{
	public:
		PostprocessingRenderPass(Ptr<SceneRenderPass> scenePass);

		void Render(Vec2 currentWindowSize);
		void UploadUniforms();

	private:
		GLuint mSceneColorTexture = 0;
		GLuint mSceneDepthTexture = 0;

		Ptr<SceneRenderPass> mScenePass = nullptr;

		SceneRenderPass::SceneRenderData& mSceneData;

		Ptr<Shader>		  mShader		= nullptr;
		Ptr<VertexBuffer> mVertexBuffer = nullptr;
		Ptr<VertexArray>  mVertexArray  = nullptr;

		int  mToneMapIndex	= 3;
		bool mUseDoF		= false;
		bool mUseMotionBlur = false;
	};
}