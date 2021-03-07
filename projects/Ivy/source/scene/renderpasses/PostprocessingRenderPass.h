#pragma once
#include "Types.h"
#include "rendering/Shader.h"
#include "rendering/Buffer.h"
#include "rendering/BufferLayout.h"
#include "rendering/VertexArray.h"
#include "SceneRenderPass.h"

namespace Ivy
{
	/*!
	 * The renderpass that handles the postprocessing stack
	 * 
	 */
	class PostprocessingRenderPass
	{
	public:
		PostprocessingRenderPass(Ptr<SceneRenderPass> scenePass);

		void Render(Vec2 currentWindowSize, float deltaTime);
		void UploadUniforms(float deltaTime);

	private:
		GLuint mSceneColorTexture = 0;
		GLuint mSceneDepthTexture = 0;

		Ptr<SceneRenderPass> mScenePass = nullptr;

		SceneRenderPass::SceneRenderData& mSceneData;

		Ptr<Shader>		  mShader		= nullptr;
		Ptr<VertexBuffer> mVertexBuffer = nullptr;
		Ptr<VertexArray>  mVertexArray  = nullptr;

		int   mToneMapIndex		   = 0;
		bool  mUseDoF			   = false;
		bool  mUseMotionBlur	   = true;
		float mMotionBlurIntensity = 5.0f;
		float mDofThreshold		   = 0.99;
	};
}