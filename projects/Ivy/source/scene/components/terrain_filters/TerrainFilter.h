#pragma once
#include "Types.h"
#include "scene/components/Component.h"

namespace Ivy
{
	class Terrain;

	class TerrainFilter : Component 
	{
	public:
		TerrainFilter(Ptr<Terrain> entity, bool generateOnStart = false) 
			: Component::Component(std::static_pointer_cast<Entity>(entity))
			, mTerrain(entity)
			, mGenerateOnStart(generateOnStart)
		{

		}

		virtual void Generate() = 0;



	protected:
		Ptr<Terrain> mTerrain = nullptr;
		
		bool mGenerateOnStart = false;

	private:

	
	};
}
