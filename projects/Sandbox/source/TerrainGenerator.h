#pragma once
#include <Ivy.h>
#include "Erosion.h"

using namespace Ivy;

class TerrainGenerator : public Entity
{
public:
	TerrainGenerator();
	void OnCreate() override;
	void OnStart() override;
	void OnUpdate(float deltaTime) override;
	void SetTerrainEntity(Ptr<Terrain> terrain) { mTerrain = terrain; }

	void Erode();
	void GenerateRiverPath();

private:
	void GenerateHeightmap();
	//void Erode();


	Ptr<Terrain> mTerrain = nullptr;
	//Ptr<Scene>   mScene   = nullptr;

	Vector<float> mHeightmap = { 0.0f };
	VecI2 mDimensions = VecI2(0.0f);

	// Erosion parameters
	//int   mIterations = 500000;

	// River generation
	Vector<Line> mRiverPathLines;


	float mSteepnessCutoff = 0.3f; 
};
