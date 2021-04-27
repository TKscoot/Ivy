#pragma once
#include <Ivy.h>
#include "Erosion.h"

using namespace Ivy;

class TerrainGenerator : public Entity
{
public:
	TerrainGenerator();
	void OnStart() override;
	void OnUpdate(float deltaTime) override;
	void SetTerrainEntity(Ptr<Terrain> terrain) { mTerrain = terrain; }

private:
	void GenerateHeightmap();
	//void Erode();


	Ptr<Terrain> mTerrain = nullptr;
	//Ptr<Scene>   mScene   = nullptr;

	Vector<float> mHeightmap = { 0.0f };
	VecI2 mDimensions = VecI2(0.0f);

	// Erosion parameters
	int   mIterations = 1000;

	int   mMaxDropletLifetime  = 30;
	float mInertia			   = 0.05f;
	float mInitialSpeed		   = 1.0f;
	float mInitialWater		   = 1.0f;
	float mInitialSediment     = 0.0f;
	float mMinSlope			   = 0.1f;
	float mMaxSedimentCapacity = 1.0f;
	float mDeposition		   = 0.8f;
	float mErosion			   = 0.01f;
	float mGravity			   = 4.0f;
	float mEvaporationSpeed = 0.01f;

	// current vars (members so i can use them with ImGui)
	float mSpeed	= 1.0f;
	float mWater	= 1.0f;
	float mSediment = 0.0f;

	//Ptr<Erosion<compute_mode_e::serial>> mEroder = nullptr;

};

struct Particle
{
	//Construct Particle at Position
	Particle(glm::vec2 _pos) { pos = _pos; }

	glm::vec2 pos;
	glm::vec2 speed = glm::vec2(0.0);

	float volume = 1.0;   //This will vary in time
	float sediment = 0.0; //Fraction of Volume that is Sediment!
};