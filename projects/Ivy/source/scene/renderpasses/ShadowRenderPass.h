#pragma once
#include "Types.h"
#include "rendering/Framebuffer.h"
#include "rendering/Shader.h"
#include "scene/Camera.h"
#include "scene/Light.h"
#include "scene/Entity.h"
#include "scene/components/Transform.h"
#include "scene/components/Mesh.h"
#include <imgui.h>

namespace Ivy
{
	class ShadowRenderPass
	{
	public:
		struct CascadeData
		{
			glm::mat4 ViewProj;
			glm::mat4 View;
			float SplitDepth;
		};

		ShadowRenderPass(Ptr<Camera> camera)
			: mCamera(camera)
		{
			mShadowFBO = CreatePtr<Framebuffer>(SHADOW_WIDTH, SHADOW_HEIGHT);

			SetupShader();
			CreateFramebuffer();
		}

		void Update();

		void RenderShadows(VecI2 windowSize, Vector<Ptr<Entity>>& entities);
		void UpdateShaderUniforms(Ptr<Shader> shader);

		void SetDirLight(DirectionalLight& light) // eventuell buggy wegen reference
		{
			mDirLight = &light;
		}

		void SetupShader();

		std::array<GLuint, 4> GetTextureIDs()	 { return mTextures; }
		std::array<Mat4, 4>   GetLightMatrices() { return mLightSpaceMatrices; }
		const unsigned int	  GetCascadeCount()  { return CASCADE_COUNT; }


	private:
		void CalculateCascades(Vec2 currentWindowSize, CascadeData* cascades, const Vec3& lightDirection);
		void CreateFramebuffer();
		void BindFboForWriting(uint32_t cascadeIndex);

		const unsigned int CASCADE_COUNT = 4;
		const unsigned int SHADOW_WIDTH	 = 4096, SHADOW_HEIGHT = 4096;

		//Ptr<Framebuffer>	mFramebuffer = nullptr;
		GLuint mFBO = 0;
		std::array<GLuint, 4> mTextures;

		Ptr<Framebuffer>	mShadowFBO   = nullptr;
		Ptr<Shader>			mDepthShader = nullptr;
		Ptr<Camera>			mCamera		 = nullptr;

		DirectionalLight*   mDirLight;
		std::array<Mat4, 4>	mLightSpaceMatrices;

		// Params
		bool mOldShadows = false;

		float mCascadeSplitLambda	 = 0.7f;
		float mCascadeFarPlaneOffset = 15.0f, mCascadeNearPlaneOffset = -15.0f;
		bool  mShowCascades			 = false;
		bool  mSoftShadows			 = true;
		bool  mCascadeFading		 = true;
		float mLightSize			 = 0.1f;
		float mMaxShadowDistance	 = 200.0f;
		float mShadowFade			 = 25.0f;
		float mCascadeTransitionFade = 1.0f;
		Vec4  mCascadeSplits;
		Mat4  mLightViewMatrix;
	};
}
