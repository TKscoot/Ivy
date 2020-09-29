#pragma once
#include <typeindex>

#include "Types.h"
#include "components/Component.h"

namespace Ivy
{
	class Entity
	{
	public:
		// Make Scene a friend of this class so it has access to private members
		// because userspace should not get access of UpdateComponents() but it needs to
		// be called in core
		friend class Scene;

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

			return component;
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

		void OnUpdate()  {};
		void OnDestroy() {};
		void OnCreate()  {};

	private:
		void UpdateComponents();
		UnorderedMap<std::type_index, Vector<Ptr<Component>>> mComponents;

		uint32_t mIndex = 0;

	};
}
