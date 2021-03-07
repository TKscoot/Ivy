
#pragma once

#include <vector>
#include <random.hpp>

using Random = effolkronium::random_static;

struct HeightAndGradient
{
	float height;
	float gradientX;
	float gradientY;
};

enum compute_mode_e { serial, parallel };

template <compute_mode_e COMPUTE_MODE = compute_mode_e::serial>
class Erosion
{
public:
	Erosion(unsigned mapSize);
	void setSeed(int seed);
	void erode(std::vector<float> &map, unsigned numIterations = 1);

private:
	std::vector<std::vector<unsigned> *> erosionBrushIndices = std::vector<std::vector<unsigned> *>();
	std::vector<std::vector<float> *> erosionBrushWeights = std::vector<std::vector<float> *>();

	unsigned mapSize;
	int seed = 1231204;
	const unsigned erosionRadius = 3;
	const float inertia = 0.05f;
	const float sedimentCapacityFactor = 4;
	const float minSedimentCapacity = 0.01f;
	const float erodeSpeed = 0.3f;
	const float depositSpeed = 0.3f;
	const float evaporateSpeed = 0.01f;
	const float gravity = 4;
	const float maxDropletLifetime = 30;

	const float initialWaterVolume = 1;
	const float initialSpeed = 1;

	void initializeBrushIndices();
	HeightAndGradient calculateHeightAndGradient(std::vector<float> &nodes, float posX, float posY);
};