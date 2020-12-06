#pragma once
#include "Types.h"
#include "scene/Entity.h"
#include "scene/Camera.h"

namespace Ivy
{
	class SceneRenderPass
	{
	public:
		SceneRenderPass(Ptr<Camera> camera, Vector<Entity>& entities);

	private:
		Ptr<Camera>		mCamera	  = nullptr;
		Vector<Entity>& mEntities = {};
	};
}
