#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator()
{
	mDimensions = { 256, 256 };
	//mTerrain = mScene->CreateEntity<Terrain>(mDimensions.x, mDimensions.y);
}

void TerrainGenerator::OnStart()
{
	GenerateHeightmap();
	//mTerrain->GetFirstComponentOfType<Transform>()->setScale(Vec3(50.0f));

	for (int i = 0; i < 30; i++)
	{
		auto ent = mScene->CreateEntity();
		auto mesh = ent->AddComponent<Mesh>(CreatePtr<Mesh>(ent.get(), "assets/models/Oak_Tree.obj"));
		auto t = ent->GetFirstComponentOfType<Transform>();
		
		float h = mTerrain->GetHeightFromWorldPos(i * 5, 0);
		Debug::Log("Height: {}", h);
		
		t->setPosition(Vec3(i *5, h, 0));
	}


}

void TerrainGenerator::OnUpdate(float deltaTime)
{
	static int erosionTime = 0.0f;
	ImGui::Begin("Erosion");
	if(ImGui::CollapsingHeader("Erosion", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginChild("Erosion", ImVec2(0, 420), true);
		ImGui::BeginChild("ErosionStats", ImVec2(0, 65), true);
		ImGui::Text("Current speed:    %.4f", mSpeed);
		ImGui::Text("Current water:    %.4f", mWater);
		ImGui::Text("Current sediment: %.4f", mSediment);
		ImGui::EndChild();
		ImGui::Spacing();

		ImGui::SliderInt("Iterations", &mIterations, 1, 30000);
		ImGui::Spacing();

		ImGui::SliderInt("MaxDropletLifetime", &mMaxDropletLifetime, 1, 100);
		ImGui::SliderFloat("Inertia", &mInertia, 0.0f, 1.0f);
		ImGui::SliderFloat("InitialSpeed", &mInitialSpeed, 0.0f, 20.0f);
		ImGui::SliderFloat("InitialWater", &mInitialWater, 0.0f, 20.0f);
		ImGui::SliderFloat("InitialSediment", &mInitialSediment, 0.0f, 1.0f);
		ImGui::SliderFloat("MinSlope", &mMinSlope, 0.0f, 20.0f);
		ImGui::SliderFloat("MaxSedimentCapacity", &mMaxSedimentCapacity, 1.0f, 64.0f);
		ImGui::SliderFloat("Deposition", &mDeposition, 0.0f, 1.0f);
		ImGui::SliderFloat("Erosion", &mErosion, 0.0f, 1.0f);
		ImGui::SliderFloat("Gravity", &mGravity, 1.0f, 10.0f);
		ImGui::SliderFloat("EvaporationSpeed", &mEvaporationSpeed, 0.0f, 1.0f);
		ImGui::Spacing();

		if(ImGui::Button("Erode!"))
		{
			auto start = std::chrono::steady_clock::now();
			
			//Erode();
		
			auto end = std::chrono::steady_clock::now();
			erosionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		}
		ImGui::SameLine();
		ImGui::Text("Calculation took %d milliseconds!", erosionTime);
		ImGui::Spacing();

		//if(ImGui::Button("Reset parameters!"))
		//{
		//	ResetParameters();
		//}
		ImGui::EndChild();
	}
	ImGui::Spacing();
	ImGui::End();
}

void TerrainGenerator::GenerateHeightmap()
{
	mHeightmap.resize(mDimensions.x * mDimensions.y);

	float xFactor = 1.0f / (mDimensions.x - 1);
	float yFactor = 1.0f / (mDimensions.y - 1);
	float baseFreq = 4.0f;
	float persistence = 0.5f;
	for(int z = 0; z < mDimensions.x; z++)
	{
		for(int x = 0; x < mDimensions.y; x++)
		{
			float hx = xFactor * x;
			float hy = yFactor * z;
			float sum = 0.0f;
			float freq = baseFreq;
			float persist = persistence;
			for(int oct = 0; oct < 4; oct++)
			{
				glm::vec2 p(hx * freq, hy * freq);

				float val = 0.0f;
				if(true)
				{
					val = glm::perlin(p, glm::vec2(freq)) * persist;
				}
				else
				{
					val = glm::perlin(p) * persist;
				}

				sum += val;

				freq *= 2.0f;
				persist *= persistence;
			}

			mHeightmap[z * mDimensions.x + x] = sum * 50.0f;
		}
	}

	for(int z = 0; z < mDimensions.x; z++)
	{
		for(int x = 0; x < mDimensions.y; x++)
		{
			mTerrain->SetHeight(x, z, mHeightmap[z*mDimensions.x + x]);
		}
	}

	Timer t;
	t.Start();
	mTerrain->CalculateNormals(0, 0);
	t.Stop();
	Debug::Info("Normals took {0:.8f}ms to calculate!", t.ElapsedMilliseconds());
	t.Start();
	mTerrain->CreateResources();
	t.Stop();
	Debug::Info("CreateResources() took {0:.8f}ms to calculate!", t.ElapsedMilliseconds());


}


/*
void TerrainGenerator::Erode()
{
	

	Erosion<compute_mode_e::parallel> eroder(mDimensions.x);
	std::srand(std::time(NULL));
	eroder.setSeed(std::rand() % 500);
	eroder.erode(mHeightmap, mIterations);

	for(int z = 0; z < mDimensions.x; z++)
	{
		for(int x = 0; x < mDimensions.y; x++)
		{
			mTerrain->SetHeight(x, z, mHeightmap[z*mDimensions.x + x]);
		}
	}

	Timer t;
	t.Start();
	mTerrain->CalculateNormals(0, 0);
	t.Stop();
	Debug::Info("Normals took {0:.8f}ms to calculate!", t.ElapsedMilliseconds());
	t.Start();
	mTerrain->CreateResources();
	t.Stop();
	Debug::Info("CreateResources() took {0:.8f}ms to calculate!", t.ElapsedMilliseconds());
}
*/