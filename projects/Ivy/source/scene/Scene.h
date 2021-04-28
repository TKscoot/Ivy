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
	/*!
	 * The Scene class
	 * Handles everything that is in the scene (Lights, Skybox, Entities, ...)
	 * 
	 */
	class Scene : public std::enable_shared_from_this<Scene>
	{
	public:

		friend class Renderer;
		friend class SceneManager;

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
		
		/*!
		 * Constructor
		 * 
		 */
		Scene(String name);
		~Scene();

		/*!
		 * Creates a default entity and adds it to the scene
		 * 
		 * \return Returns pointer to the created entity
		 */
		Ptr<Entity> CreateEntity();
		
		/*!
		 * Creates Entity with custom type
		 * 
		 * \param ...args
		 * \return Returns pointer to the created entity
		 */
		template <typename T, typename... Args>
		Ptr<T> CreateEntity(Args&&... args)
		{
			Ptr<T> ent = CreatePtr<T>(std::forward<Args>(args)...);
			ent->mIndex = mEntities.size();
			ent->mScene = shared_from_this();
			ent->AddComponent(CreatePtr<Transform>());
			ent->AddComponent(CreatePtr<Material>());
			ent->OnCreate();
			Debug::CoreLog("Created entity with index {}", ent->mIndex);

			mEntities.push_back(ent);
			return ent;
		}

		/*!
		 * Sets skybox from vector of filepaths to the textures
		 * Order in which the texture must be: right, left, top, bottom, back, front
		 * 
		 * \param filepaths Filepaths to the textures
		 */
		void SetSkybox(Vector<String> filepaths);

		/*!
		 * Sets Skybox from texturepaths
		 * 
		 * \param right Right side texture path
		 * \param left Left side texture path
		 * \param top Top side texture path
		 * \param bottom Bottom side texture path
		 * \param back Back side texture path
		 * \param front Front side texture path
		 */
		void SetSkybox(String right,
			String left,
			String top,
			String bottom,
			String back,
			String front);

		void SetHdriEnvironment(String path);

		/*!
		 * Updates the scene data
		 * Internal use! Dont call this!
		 * 
		 * \param deltaTime
		 */
		void Update(float deltaTime);

		/*!
		* Renders the scene objects
		* Internal use! Dont call this!
		*
		*/
		void Render(float deltaTime, Vec2 currentWindowSize);

		/*!
		 * Gets all entities. Including inactives.
		 * 
		 * \return Vector of the entity pointers
		 */
		Vector<Ptr<Entity>> GetEntities() { return mEntities; }

		/*!
		 * Gets specific entity with index
		 * 
		 * \param entityIndex The index of the entity
		 * \return Pointer to entity
		 */
		Ptr<Entity> GetEntityWithIndex(uint32_t entityIndex)
		{
			return mEntities.at(entityIndex);
		}

		/*!
		 * Gets the camera object
		 * 
		 * \return Pointer to the Camera
		 */
		Ptr<Camera> GetCamera() { return mCamera; }

		/*!
		 * Adds directional light
		 * 
		 * \param lightParams Parameters for the light
		 * \return 
		 */
		DirectionalLight& AddDirectionalLight(DirectionalLight lightParams);
		/*!
		 * Adds directional light
		 * 
		 * \param direction The direction
		 * \param ambient Ambient color
		 * \param diffuse Diffuse color
		 * \param specular Specular color
		 * \return 
		 */
		DirectionalLight& AddDirectionalLight(float intensity,
											  Vec3 direction,
											  Vec3 ambient,
											  Vec3 diffuse,
											  Vec3 specular);

		/*!
		 * Adds point light
		 * 
		 * \param lightParams Parameters for the light
		 * \return 
		 */
		PointLight& AddPointLight(PointLight lightParams);
		PointLight& AddPointLight(Vec3 position,
							      float constant,
							      float linear,
							      float quadratic,
							      Vec3 ambient,
							      Vec3 diffuse,
							      Vec3 specular);
		/*!
		 * Adds spot light
		 *
		 * \param lightParams Parameters for the light
		 * \return
		 */
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

		/*!
		 * Sets the direction of the light
		 * 
		 * \param direction Direction as vector
		 */
		void SetDirectionalLightDirection(Vec3 direction) 
		{
			mDirLight.direction = direction;
			mCSM->SetDirLight(mDirLight);
		}

		/*!
		 * Internal!
		 * 
		 */
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

		/*!
		 * Internal!
		 *
		 */
		void InitializeGUI(Ptr<Window> window) 
		{
			mImGuiHook = CreatePtr<ImGuiHook>(window);
		}

		/*!
		 * Internal!
		 *
		 */
		void InitializeScenePass(Ptr<Window> window)
		{
			mScenePass = CreatePtr<SceneRenderPass>(
				mCamera,
				window,
				mEntities,
				mCSM,
				mSkyModel,
				mDirLight,
				mSpotLights,
				mPointLights);

			mPostprocessPass = CreatePtr<PostprocessingRenderPass>(mScenePass);
		}
		 /*!
		  * Toggles rendering of the GUI
		  * 
		  */
		void ToggleGUI() { mRenderGui = !mRenderGui; }


		String GetName()
		{
			return mName;
		}

		bool IsActiveScene() { return mIsActiveScene; }

	private:
		void Load();
		void Unload();

		String mName = "";
		bool mIsActiveScene = false;

		Vector<String> mSkyboxPaths;
		String mEnvMapPath = "";
		bool mUseSkybox = false;
		bool mUseEnvMap = false;

		bool mFirstUpdate = true;

		GLuint mQuery = 0;


		static Ptr<Scene>   mInstance;

		Vector<Ptr<Entity>>   mEntities	 = {};
		Ptr<Camera>           mCamera	 = nullptr;
		Ptr<ShadowRenderPass> mCSM		 = nullptr;
		Ptr<SceneRenderPass>  mScenePass = nullptr;
		Ptr<PostprocessingRenderPass> mPostprocessPass = nullptr;
		Ptr<ImGuiHook>		  mImGuiHook = nullptr;
		Ptr<SkyModel>		  mSkyModel  = nullptr;


		// Lights
		DirectionalLight	mDirLight	 = {};
		Vector<SpotLight>	mSpotLights  = {};
		Vector<PointLight>	mPointLights = {};

		bool mRenderGui = true;
	};
}
