#pragma once
#include "Entity.h"
#include "components/Transform.h"
#include "components/Material.h"
#include "components/Mesh.h"
#include "Camera.h"

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

	private:
		void SetupSkybox();
		void SetupSkyboxShaders();

		static Ptr<Scene>   mInstance;
		Vector<Ptr<Entity>> mEntities = {};
		Ptr<Camera>         mCamera;

		bool			    mShouldRenderSkybox = false;
		Vector<String>		mSkyboxFilepaths	= {};

		Ptr<TextureCube>    mSkyboxCubeTexture	= nullptr;
		Ptr<Shader>			mSkyboxShader		= nullptr;
		Ptr<VertexBuffer>   mSkyboxVertexBuffer = nullptr;
		Ptr<VertexArray>    mSkyboxVertexArray	= nullptr;
	};
}