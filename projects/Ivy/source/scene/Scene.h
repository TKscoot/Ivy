#pragma once
#include "Entity.h"
#include "components/Transform.h"
#include "components/Material.h"
#include "components/Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "rendering/Framebuffer.h"

namespace Ivy
{
	class Scene
	{
	public:
		
		Scene();
		~Scene();

		Ptr<Entity> CreateEntity();

		void SetSkybox(Vector<String> filepaths);
		void SetSkybox(String right,
			String left,
			String top,
			String bottom,
			String back,
			String front);

		void Update();

		void Render(float deltaTime, Vec2 currentWindowSize);
		
		static Ptr<Scene> GetScene()
		{
			if (!mInstance)
			{
				mInstance = CreatePtr<Scene>();
			}

			return mInstance;
		}

		Vector<Ptr<Entity>> GetEntities() { return mEntities; }

		Ptr<Entity> GetEntityWithIndex(uint32_t entityIndex)
		{
			return mEntities.at(entityIndex);
		}

		Ptr<Camera> GetCamera() { return mCamera; }

		DirectionalLight& AddDirectionalLight(DirectionalLight lightParams);
		DirectionalLight& AddDirectionalLight(Vec3 direction,
											  Vec3 ambient,
											  Vec3 diffuse,
											  Vec3 specular);

		PointLight& AddPointLight(PointLight lightParams);
		PointLight& AddPointLight(Vec3 position,
							      float constant,
							      float linear,
							      float quadratic,
							      Vec3 ambient,
							      Vec3 diffuse,
							      Vec3 specular);

		SpotLight& AddSpotLight(SpotLight& lightParams);
		SpotLight& AddSpotLight(Vec3 position,
							    Vec3 direction,
							    float cutOff,
							    float outerCutOff,
							    float constant,
							    float linear,
							    float quadratic,
							    Vec3 ambient,
							    Vec3 diffuse,
							    Vec3 specular);

		void SetDirectionalLightDirection(Vec3 direction) { mDirLight.direction = direction; }

	private:
		void SetupSkybox();
		void SetupSkyboxShaders();
		void PushLightParams(Ptr<Shader> shader);

		void RenderEntities();

		void SetupShadows();
		void RenderShadows();

		static Ptr<Scene>   mInstance;
		Vector<Ptr<Entity>> mEntities = {};
		Ptr<Camera>         mCamera = nullptr;

		// Lights
		DirectionalLight	mDirLight	 = {};
		Vector<SpotLight>	mSpotLights  = {};
		Vector<PointLight>	mPointLights = {};

		// Skybox
		bool			    mShouldRenderSkybox = false;
		Vector<String>		mSkyboxFilepaths	= {};
		Ptr<TextureCube>    mSkyboxCubeTexture	= nullptr;
		Ptr<Shader>			mSkyboxShader		= nullptr;
		Ptr<VertexBuffer>   mSkyboxVertexBuffer = nullptr;
		Ptr<VertexArray>    mSkyboxVertexArray	= nullptr;

		// Shadows
		const unsigned int SHADOW_WIDTH  = 1024, SHADOW_HEIGHT = 1024;
		Ptr<Framebuffer>	mShadowFBO   = nullptr;
		Ptr<Shader>			mDepthShader = nullptr;
		Mat4 lightSpaceMatrix;

	};
}