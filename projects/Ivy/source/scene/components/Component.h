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

		void SetEntityIndex(uint32_t index) { mEntityIndex = index; }

		uint32_t GetEntityIndex()
		{
			return mEntityIndex;
		}

		virtual ~Component() = default;
	private:
		bool mActive = true;
		uint32_t mEntityIndex = 0;
	};
}