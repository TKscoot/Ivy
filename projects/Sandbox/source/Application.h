#pragma once
#include <Ivy.h>
#include "CameraTracker.h"

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

	Ptr<Entity> mBouncingEntity = nullptr;

};