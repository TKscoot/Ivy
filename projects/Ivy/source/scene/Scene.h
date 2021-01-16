#pragma once
#include "Entity.h"
#include "components/Transform.h"
#include "components/Material.h"
#include "components/Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "rendering/Framebuffer.h"
#include "renderpasses/ShadowRenderPass.h"
#include "renderpasses/SceneRenderPass.h"
#include "renderpasses/PostprocessingRenderPass.h"
#include "rendering/ImGuiHook.h"

namespace Ivy
{
	class Scene
	{
	public:

		friend class Renderer;

		struct FrustumBounds
		{
			float r, l, b, t, f, n;
		};

		struct CascadeData
		{
			glm::mat4 ViewProj;
			glm::mat4 View;
			float SplitDepth;
		};
		
		Scene();
		~Scene();

		Ptr<Entity> CreateEntity();
		
		template <typename T, typename... Args>
		Ptr<T> CreateEntity(Args&&... args)
		{
			Ptr<T> ent = CreatePtr<T>(std::forward<Args>(args)...);
			ent->mIndex = mEntities.size();
			ent->AddComponent(CreatePtr<Transform>());
			ent->AddComponent(CreatePtr<Material>());
			ent->OnCreate();
			Debug::CoreLog("Created entity with index {}", ent->mIndex);

			mEntities.push_back(ent);
			return ent;
		}


		void SetSkybox(Vector<String> filepaths);
		void SetSkybox(String right,
			String left,
			String top,
			String bottom,
			String back,
			String front);

		void Update(float deltaTime);

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

		void SortEntitiesForAlpha()
		{
			for(auto& ent : mEntities)
			{
				ent->mCamera = mCamera;
				Debug::CoreLog("Distance to entity {} is {}", ent->mIndex, ent->GetCameraDistance());
			}
			std::sort(mEntities.begin(), mEntities.end(), Entity::alpha_sort_key());
			for(auto& ent : mEntities)
			{
				Debug::CoreLog("sorted: {}", ent->mIndex);
			}
		}

		void InitializeGUI(Ptr<Window> window) 
		{
			mImGuiHook = CreatePtr<ImGuiHook>(window->GetHandle());
		}
		void InitializeScenePass(Ptr<Window> window)
		{
			mScenePass = CreatePtr<SceneRenderPass>(
				mCamera,
				window,
				mEntities,
				mCSM,
				mDirLight,
				mSpotLights,
				mPointLights);

			mPostprocessPass = CreatePtr<PostprocessingRenderPass>(mScenePass);
		}

		void ToggleGUI() { mRenderGui = !mRenderGui; }
	private:



		static Ptr<Scene>   mInstance;

		Vector<Ptr<Entity>>   mEntities	 = {};
		Ptr<Camera>           mCamera	 = nullptr;
		Ptr<ShadowRenderPass> mCSM		 = nullptr;
		Ptr<SceneRenderPass>  mScenePass = nullptr;
		Ptr<PostprocessingRenderPass> mPostprocessPass = nullptr;
		Ptr<ImGuiHook>		  mImGuiHook = nullptr;

		// Lights
		DirectionalLight	mDirLight	 = {};
		Vector<SpotLight>	mSpotLights  = {};
		Vector<PointLight>	mPointLights = {};

		bool mRenderGui = true;
	};
}