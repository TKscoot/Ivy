#include "ivypch.h"
#include "SceneManager.h"

Ivy::Ptr<Ivy::SceneManager> Ivy::SceneManager::mInstance = nullptr;

Ivy::SceneManager::SceneManager()
{
}

Ivy::Ptr<Ivy::Scene> Ivy::SceneManager::CreateScene(String name)
{
	Ptr<Scene> scene = CreatePtr<Scene>(name);
	mScenes[name] = scene;

	//mScenes[name]->InitializeGUI(mWindow);
	mScenes[name]->InitializeScenePass(mWindow);

	for (auto& c : mSceneCreateCallbacks)
	{
		c(mScenes[name]);
	}

	return scene;
}

Ivy::Ptr<Ivy::Scene> Ivy::SceneManager::CreateSceneAndSetActive(String name)
{
	CreateScene(name);
	SetActiveScene(name);
	return mScenes[name];
}

Ivy::Ptr<Ivy::Scene> Ivy::SceneManager::GetScene(String name)
{
	if(mScenes.find(name) == mScenes.end())
	{
		Debug::CoreError("\"{}\" is not a valid scene!");
		return nullptr;
	}

	return mScenes[name];
}

void Ivy::SceneManager::SetActiveScene(String name)
{
	if (mScenes[name] == nullptr)
	{
		Debug::CoreError("Scene \"{}\" does not exist or is nullptr! Not changing scene!", name);
		return;
	}

	if(mActiveScene)
	{
		mActiveScene->Unload();
		mActiveScene->mIsActiveScene = false;

		for(auto& e : mActiveScene->mEntities)
		{
			e->mAttachedSceneIsActive = false;
		}

	}

	mActiveScene = mScenes[name];
	mActiveScene->mIsActiveScene = true;
	for(auto& e : mActiveScene->mEntities)
	{
		e->mAttachedSceneIsActive = true;
	}
	mActiveScene->Load();
	for(auto& c : mSceneLoadCallbacks)
	{
		c(mActiveScene);
	}
}

void Ivy::SceneManager::SetActiveScene(Ptr<Scene> scene)
{
	if(!scene)
	{
		Debug::CoreError("Couldn't set scene as active! Scene is not valid or null");
		return;
	}

	if(mActiveScene)
	{
		mActiveScene->Unload();
		mActiveScene->mIsActiveScene = false;

		for(auto& e : mActiveScene->mEntities)
		{
			e->mAttachedSceneIsActive = false;
		}

	}

	mActiveScene = mScenes[scene->GetName()];
	mActiveScene->mIsActiveScene = true;
	for(auto& e : mActiveScene->mEntities)
	{
		e->mAttachedSceneIsActive = true;
	}
	mActiveScene->Load();

}

Ivy::Ptr<Ivy::Scene> Ivy::SceneManager::GetActiveScene()
{
	if(!mActiveScene)
	{
		Debug::CoreError("No scene is set as active!");
	}

	return mActiveScene;
}
