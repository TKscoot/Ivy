#pragma once
#include "scene/components/Mesh.h"

namespace Ivy
{
	class Cube : public Mesh, public Component
	{
	public:
		Cube(Entity* ent);

	private:
		Submesh mSubMesh;
	};
}