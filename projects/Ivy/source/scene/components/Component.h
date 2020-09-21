#pragma once

namespace Ivy
{
	class Entity;
	class Component
	{
	public:
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

		
		void SetEntity(Ptr<Entity> ent)
		{
			mEntity = ent;
		}

		Ptr<Entity> GetEntity()
		{
			return mEntity;
		}

		virtual ~Component() = default;
	private:
		bool mActive = true;
		Ptr<Entity> mEntity;
	};
}