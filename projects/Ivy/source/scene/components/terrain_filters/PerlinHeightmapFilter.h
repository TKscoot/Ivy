#pragma once
#include "TerrainFilter.h"
namespace Ivy
{
	class PerlinHeightmapFilter : public TerrainFilter
	{
	public:
		PerlinHeightmapFilter(Ptr<Terrain> entity, bool generateOnStart = false)
			: TerrainFilter(entity, generateOnStart)
		{
			mTerrain = entity;
			mGenerateOnStart = generateOnStart;


		}

		void Generate() override;

	private:

	};

}