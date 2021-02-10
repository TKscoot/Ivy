#pragma once
#include "Types.h"
#include "scene/Entity.h"
#include "scene/Camera.h"
#include "scene/components/Material.h"
#include "scene/components/Mesh.h"
#include "scene/components/Transform.h"
#include "scene/renderpasses/ShadowRenderPass.h"
#include "rendering/Line.h"

namespace Ivy
{
	class SceneRenderPass
	{
	public:
		struct SceneRenderData
		{
			Mat4 world;
			Mat4 view;
			Mat4 proj;
			Mat4 wvp;
			Mat4 invViewProj;
			Mat4 previousProj;
			Mat4 previousView;
			Mat4 previousViewProj;

			Vec2 windowResolution;

			Vec3 cameraPosition;
			Vec3 previousCameraPosition;

			float nearPlane;
			float farPlane;
		};
	
		SceneRenderPass(Ptr<Camera> camera, 
			Ptr<Window>				window,
			Vector<Ptr<Entity>>&	entities,
			Ptr<ShadowRenderPass>   shadowPass,
			DirectionalLight&		dirLight,
			Vector<SpotLight>&		spotLights,
			Vector<PointLight>&		pointLights);

		void Render(Vec2 currentWindowSize);

		void SetupSkybox(Vector<String> filepaths);

		GLuint GetColorTextureID()
		{
			return mColorTexture;
		}

		GLuint GetGodrayOcclusionTextureID()
		{
			return mGodrayOcclusionTexture;
		}

		GLuint GetDepthTextureID()
		{
			return mDepthTexture;
		}

		SceneRenderData& GetSceneRenderData() { return mRenderData; }

	private:
		void PushLightParams(Ptr<Shader> shader);
		void SetupSkyboxShaders();

		void SetupFramebuffer();

		void BindFramebufferForWrite();




		Ptr<Camera>			  mCamera = nullptr;
		Ptr<Window>			  mWindow = nullptr;
		Vector<Ptr<Entity>>&  mEntities;

		Ptr<ShadowRenderPass> mCSM = nullptr;

		VecI2				  mWindowSize = Vec2(0.0f);

		// FBO
		GLuint mFBO = 0;
		GLuint mRBO = 0;
		GLuint mDepthTexture = 0;
		GLuint mColorTexture = 0;
		GLuint mGodrayOcclusionTexture = 0;

		SceneRenderData mRenderData;

		// Lights
		DirectionalLight&	mDirLight;
		Vector<SpotLight>&	mSpotLights;
		Vector<PointLight>&	mPointLights;

		// Skybox
		bool			    mShouldRenderSkybox = false;
		Vector<String>		mSkyboxFilepaths = {};
		Ptr<TextureCube>    mSkyboxCubeTexture = nullptr;
		Ptr<Shader>			mSkyboxShader = nullptr;
		Ptr<VertexBuffer>   mSkyboxVertexBuffer = nullptr;
		Ptr<VertexArray>    mSkyboxVertexArray = nullptr;


	};
}
