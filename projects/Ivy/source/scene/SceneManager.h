#pragma once
#include "Scene.h"

namespace Ivy
{
	class SceneManager
	{
	public:


		SceneManager();

		static Ptr<SceneManager> GetInstance()
		{
			if(!mInstance)
			{
				mInstance = CreatePtr<SceneManager>();
			}

			return mInstance;
		}

		Ptr<Scene> CreateScene(String name);
		Ptr<Scene> CreateSceneAndSetActive(String name);

		Ptr<Scene> GetScene(String name);

		void SetActiveScene(String name);

		void SetActiveScene(Ptr<Scene> scene);

		Ptr<Scene> GetActiveScene();
		
		void SetWindow(Ptr<Window> window) { mWindow = window; }

		void RegisterSceneLoadCallback(std::function<void(Ptr<Scene>)> callback)
		{
			mSceneLoadCallbacks.push_back(callback);
		}

		void RegisterSceneCreateCallback(std::function<void(Ptr<Scene>)> callback)
		{
			mSceneCreateCallbacks.push_back(callback);
		}

	private:
		friend class Renderer;

		static Ptr<SceneManager> mInstance;

		std::unordered_map<String, Ptr<Scene>> mScenes = {};

		Ptr<Scene> mActiveScene = nullptr;
		Ptr<Window> mWindow = nullptr;

		// Callbacks
		Vector<std::function<void(Ptr<Scene>)>> mSceneLoadCallbacks;
		Vector<std::function<void(Ptr<Scene>)>> mSceneCreateCallbacks;
	};
}