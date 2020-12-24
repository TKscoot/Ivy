#pragma once
#include <typeindex>

#include "Types.h"
#include "components/Component.h"
#include "components/Transform.h"
#include "scene/Camera.h"

namespace Ivy
{
	class Entity : public std::enable_shared_from_this<Entity>
	{
	public:
		// Make Scene a friend of this class so it has access to private members
		// because userspace should not get access of UpdateComponents() but it needs to
		// be called in core
		friend class Scene;

		float GetCameraDistance()
		{
			Vec3 camPos = mCamera->GetPosition();
			Vec3 position = GetFirstComponentOfType<Transform>()->getPosition();
			float dist = glm::length2(position - camPos);

			return dist;
		}

		bool operator<(Entity& that)
		{
			// Sort in reverse order : far particles drawn first.
			return this->GetCameraDistance() > that.GetCameraDistance();
		}

		struct alpha_sort_key
		{
			inline bool operator() (Ptr<Ivy::Entity> entity1, Ptr<Ivy::Entity> entity2)
			{
				return (entity1->GetCameraDistance() > entity2->GetCameraDistance());
			}
		};

		template <typename T>
		Ptr<T> AddComponent(Ptr<T> component)
		{
			std::type_index const index = std::type_index(typeid(T));
			if (mComponents.end() == mComponents.find(index))
			{
				mComponents[index] = {};
			}

			mComponents[index].push_back(component);
			component->SetEntityIndex(mIndex);
			component->SetEntity(this);

			return component;
		}

		template <typename T>
		void RemoveComponentsOfType()
		{
			std::type_index const index = std::type_index(typeid(T));
			if (mComponents.end() == mComponents.find(index))
			{
				mComponents[index] = {};
			}

			mComponents[index].clear();
		}

		template <typename T>
		Vector<Ptr<T>> const GetComponentsOfType()
		{
			std::type_index const index = std::type_index(typeid(T));
			if (mComponents.end() == mComponents.find(index))
			{
				return {};
			}

			Vector<Ptr<Component>> const components = mComponents.at(index);
			Vector<Ptr<T>> casted{};
			std::transform(components.begin(), components.end(), std::back_inserter(casted), 
				[](Ptr<Component> input) -> Ptr<T> 
				{ 
					return std::dynamic_pointer_cast<T>(input); 
				});

			return casted;
		}

		template<typename T>
		Ptr<T> const GetFirstComponentOfType()
		{
			std::type_index const index = std::type_index(typeid(T));
			if (mComponents.end() == mComponents.find(index))
			{
				return nullptr;
			}

			Vector<Ptr<Component>> const components = mComponents.at(index);
			Vector<Ptr<T>> casted{};
			std::transform(components.begin(), components.end(), std::back_inserter(casted),
				[](Ptr<Component> input) -> Ptr<T>
			{
				return std::dynamic_pointer_cast<T>(input);
			});

			return casted.front();
		}

		virtual void OnUpdate(float deltaTime)  {};
		virtual void OnDestroy() {};
		virtual void OnCreate()  {};

	private:
		void UpdateComponents(float deltaTime);
		UnorderedMap<std::type_index, Vector<Ptr<Component>>> mComponents;
		
		uint32_t mIndex = 0;
		Ptr<Camera> mCamera = nullptr;

	};
}
