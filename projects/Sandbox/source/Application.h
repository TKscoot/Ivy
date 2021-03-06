#pragma once
#include <Ivy.h>
#include "CameraTracker.h"
#include "TerrainGenerator.h"
#include "AudioTest.h"

using namespace Ivy;

class Application
{
public:
	Application();

	void Run();

private:
	void SetupScene();
	void SetupEntities();

	Ptr<Engine> mEngine = nullptr;
	Ptr<Scene>  mScene = nullptr;
	Ptr<Scene>  mScene2 = nullptr;

	Ptr<Entity> mBouncingEntity = nullptr;

	Ptr<Entity> sponzaEntity = nullptr;
	Ptr<Transform> sponzaTransform = nullptr;
	
	Vector<Ptr<Material>> sceneBaseMaterials;
};