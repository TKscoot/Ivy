#pragma once
#include "Entity.h"
#include "components/Transform.h"
#include "components/Material.h"

namespace Ivy
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		Ptr<Entity> CreateEntity();

		void Update();

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
		static Ptr<Scene>   mInstance;
		Vector<Ptr<Entity>> mEntities = {};

	};
}