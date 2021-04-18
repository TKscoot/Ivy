#pragma once

namespace Ivy
{
	class Entity;

	/*!
	 * Base component class
	 * Inherit from it to use a class as a component
	 * 
	 */
	class Component
	{
	public:
		/*!
		 * Default constructor
		 * 
		 */
		Component() {}

		/*!
		 * Constructor
		 * 
		 * \param ent Entity wich uses this component
		 */
		Component(Entity* ent)
		{
			mEntity = ent;
		}

		/*!
		 * Gets called when scene is being set to active and loaded
		 * Load your resources here!! Else they will be loaded on program start!
		 * 
		 */
		virtual void OnSceneLoad() {};

		/*!
		 * Gets called when scene is being unloaded and other scene gets loaded
		 * Unload your resources here!!
		 *
		 */
		virtual void OnSceneUnload() {};

		/*!
		 * Gets called when component is being created
		 * 
		 */
		virtual void OnCreate() {};

		/*!
		 * Gets called once the application starts and the scene has been loaded
		 * 
		 */
		virtual void OnStart()  {};

		/*!
		 * Gets called every frame
		 *
		 */
		virtual void OnUpdate(float deltaTime) {};


		/*!
		 * Gets called once the component or it's entity is being destroyed
		 * 
		 */
		virtual void OnDestroy(){};

		/*!
		 * Checks if the component should currently be used
		 * 
		 * \return Returns true if it's activated
		 */
		bool IsActive() 
		{ 
			return mActive; 
		}

		/*!
		 * Activates/Deactivates the component
		 * 
		 * \param active Should the component be active
		 */
		void SetActive(bool active = true)
		{
			mActive = active;
		}

		/*!
		 * Internal
		 * 
		 * \param index
		 */
		void SetEntityIndex(uint32_t index) { mEntityIndex = index; }

		/*!
		 * Internal
		 * 
		 * \return 
		 */
		uint32_t GetEntityIndex()
		{
			return mEntityIndex;
		}

		/*!
		 * Sets the entity which should use this component
		 * 
		 * \param ent The entity with this component attached
		 */
		void SetEntity(Entity* ent) { mEntity = ent; }

		/*!
		 * Destructor
		 * 
		 */
		virtual ~Component() = default;

	protected:
		Entity* mEntity = nullptr;

	private:
		bool mActive = true;
		uint32_t mEntityIndex = 0;
	};
}