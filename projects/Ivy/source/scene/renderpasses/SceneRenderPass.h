#pragma once
#include "Types.h"
#include "scene/Entity.h"
#include "scene/Camera.h"
#include "scene/components/Material.h"
#include "scene/components/Mesh.h"
#include "scene/components/Transform.h"
#include "scene/renderpasses/ShadowRenderPass.h"
#include "rendering/Line.h"
#include "skymodels/HosekWilkieSkyModel.h"

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

		struct CloudsData 
		{
			float time	  = 0.0;
			float cirrus  = 0.4;
			float cumulus = 0.8;
		};
	
		SceneRenderPass(
			Ptr<Camera>				camera, 
			Ptr<Window>				window,
			Vector<Ptr<Entity>>&	entities,
			Ptr<ShadowRenderPass>   shadowPass,
			Ptr<SkyModel>			skyModel,
			DirectionalLight&		dirLight,
			Vector<SpotLight>&		spotLights,
			Vector<PointLight>&		pointLights);

		void Render(float deltaTime, Vec2 currentWindowSize, bool renderHosekSky);

		void SetupSkybox(Vector<String> filepaths);
		void SetupSkybox(Ptr<TextureCube> tex);

		void SetEnvironmentMap(String filepath);
		void SetEnvironmentMap(Ptr<TextureCube> cubemap);

		void SetupSkyModel();

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

		void UnloadEnvironmentMap();
		void DestroySkybox();

		void SetSkyModel(Ptr<SkyModel> model) { mSkyModel = model; }
		
		void RenderSkyModelToCubemap();
		void ComputeEnvironmentMap();

		CloudsData& GetCloudsData() { return mCloudsData; }

	private:
		void PushLightParams(Ptr<Shader> shader);
		void SetupSkyboxShaders();


		void SetupFramebuffer();

		void BindFramebufferForWrite();



		Ptr<SkyModel> mSkyModel = nullptr;

		
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
		Vector<String>		mSkyboxFilepaths	= {};
		Ptr<TextureCube>    mSkyboxCubeTexture	= nullptr;
		Ptr<Shader>			mSkyboxShader		= nullptr;
		Ptr<VertexBuffer>   mSkyboxVertexBuffer = nullptr;
		Ptr<VertexArray>    mSkyboxVertexArray	= nullptr;
		bool			    mUseHdri = false;
		CloudsData			mCloudsData{};



		// IBL
		Ptr<Shader>		 mEquirectangularConversionShader = nullptr;
		Ptr<Shader>		 mEnvFilteringShader			  = nullptr;
		Ptr<Shader>		 mIrradianceShader				  = nullptr;
		Ptr<TextureCube> mEnvUnfiltered					  = nullptr;
		Ptr<TextureHDRI> mEnvEquirect					  = nullptr;
		Ptr<TextureCube> mEnvFiltered					  = nullptr;
		Ptr<TextureCube> mIrradiance					  = nullptr;
	};
}
