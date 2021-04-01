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
	mActiveScene = mScenes[name];
}

void Ivy::SceneManager::SetActiveScene(Ptr<Scene> scene)
{
	if(!scene)
	{
		Debug::CoreError("Couldn't set scene as active! Scene is not valid or null");
		return;
	}

	if(mScenes.find(scene->GetName()) == mScenes.end())
	{
		mScenes[scene->GetName()] = scene;
	}

	mActiveScene = mScenes[scene->GetName()];
}

Ivy::Ptr<Ivy::Scene> Ivy::SceneManager::GetActiveScene()
{
	if(!mActiveScene)
	{
		Debug::CoreError("No scene is set as active!");
	}

	return mActiveScene;
}
