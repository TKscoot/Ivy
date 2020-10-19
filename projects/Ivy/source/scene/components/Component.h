#pragma once

namespace Ivy
{
	class Entity;

	class Component
	{
	public:
		Component() {}

		Component(Entity* ent)
		{
			mEntity = ent;
		}

		virtual void OnCreate() {};
		virtual void OnStart()  {};
		virtual void OnUpdate() {};
		virtual void OnDestroy(){};

		bool IsActive() 
		{ 
			return mActive; 
		}

		void SetActive(bool active = true)
		{
			mActive = active;
		}

		void SetEntityIndex(uint32_t index) { mEntityIndex = index; }

		uint32_t GetEntityIndex()
		{
			return mEntityIndex;
		}

		void SetEntity(Entity* ent) { mEntity = ent; }

		virtual ~Component() = default;

	protected:
		Entity* mEntity = nullptr;

	private:
		bool mActive = true;
		uint32_t mEntityIndex = 0;
	};
}