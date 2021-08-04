#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator()
{
	mDimensions = { 1024, 1024 };
}

void TerrainGenerator::OnCreate()
{
	mTerrain = mScene->CreateEntity<Terrain>(mDimensions.x, mDimensions.y);
	mTerrain->GetMaterial()->AddUniform<float>("steepnessCutoff", &mSteepnessCutoff);
}

void TerrainGenerator::OnStart()
{
	GenerateHeightmap();

	Timer t;
	t.Start();
	Erode();
	t.Stop();

	Debug::CoreInfo("Erosion took {}ms", t.ElapsedMilliseconds());

	//mTerrain->GetFirstComponentOfType<Transform>()->setScale(Vec3(50.0f));

	for (int i = 0; i < 30; i++)
	{
		auto ent = mScene->CreateEntity();
		auto mesh = ent->AddComponent<Mesh>("assets/models/Oak_Tree.obj");
		auto t = ent->GetFirstComponentOfType<Transform>();



		auto transform = mTerrain->GetFirstComponentOfType<Transform>();

		Vec3 worldPos = Vec3(i * 5,  0.0f, 0.0f) - transform->getPosition();


		Vec3 twd;
		twd.x = (mDimensions.x - 1) * 250.0f;
		twd.y = 0.0f;
		twd.z = (mDimensions.y - 1) * 250.0f;

		Vec3 pos;
		pos.x = ((worldPos.x - transform->getPosition().x) / twd.x);// * mWidth;
		pos.z = ((worldPos.z - transform->getPosition().z) / twd.z);// * mHeight;

		Vec3 terPos;
		terPos.x = mDimensions.x * pos.x;
		terPos.z = mDimensions.y * pos.z;

		terPos.x = (i * 5 + 0.5f * ((mDimensions.x - 1) * 250.0f)) / 250.0f;
		terPos.z = (0.0f - 0.5f * ((mDimensions.y - 1) * 250.0f)) / -250.0f;

		int x = glm::floor(terPos.x);
		int z = glm::floor(terPos.z);
		
		float h = mTerrain->GetHeightFromWorldPos(i * 5, 0);
		h = (Vec3(i*5, h, 0) * Mat3(mTerrain->GetFirstComponentOfType<Transform>()->getComposed())).y;

		//h = mHeightmap[x * mDimensions.x + z];
		Debug::Log("Height: {}", h);
		
		t->setPosition(Vec3(i *5, h, 0));
	}


}

void TerrainGenerator::OnUpdate(float deltaTime)
{
	static int erosionTime = 0.0f;
	ImGui::Begin("Erosion");
	ImGui::SliderFloat("Steepness cutoff", &mSteepnessCutoff, 0.0, 1.0);

	if (ImGui::Button("Generate")) {
		GenerateHeightmap();
	}

	if(ImGui::CollapsingHeader("Erosion"))
	{
		ImGui::BeginChild("Erosion", ImVec2(0, 420), true);

		//ImGui::SliderInt("Iterations", &mIterations, 1, 30000);
		ImGui::Spacing();

		//ImGui::SliderInt("MaxDropletLifetime", &mMaxDropletLifetime, 1, 100);
		//ImGui::SliderFloat("Inertia", &mInertia, 0.0f, 1.0f);
		//ImGui::SliderFloat("InitialSpeed", &mInitialSpeed, 0.0f, 20.0f);
		//ImGui::SliderFloat("InitialWater", &mInitialWater, 0.0f, 20.0f);
		//ImGui::SliderFloat("InitialSediment", &mInitialSediment, 0.0f, 1.0f);
		//ImGui::SliderFloat("MinSlope", &mMinSlope, 0.0f, 20.0f);
		//ImGui::SliderFloat("MaxSedimentCapacity", &mMaxSedimentCapacity, 1.0f, 64.0f);
		//ImGui::SliderFloat("Deposition", &mDeposition, 0.0f, 1.0f);
		//ImGui::SliderFloat("Erosion", &mErosion, 0.0f, 1.0f);
		//ImGui::SliderFloat("Gravity", &mGravity, 1.0f, 10.0f);
		//ImGui::SliderFloat("EvaporationSpeed", &mEvaporationSpeed, 0.0f, 1.0f);
		//ImGui::Spacing();

		if(ImGui::Button("Erode!"))
		{
			auto start = std::chrono::steady_clock::now();
			
			Erode();
		
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
	Timer t;
	t.Start();

	mHeightmap.clear();
	mHeightmap.resize(mDimensions.x * mDimensions.y);

	srand(time(NULL));
	int seed = rand() % 1000;

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
			for(int octave = 0; octave < 2; octave++)
			{
				glm::vec2 p(hx * freq, hy * freq);
				p.x += seed;
				p.y += seed;

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


	mTerrain->CalculateNormals();
	t.Stop();
	Debug::Info("Normals took {0:.8f}ms to calculate!", t.ElapsedMilliseconds());
	t.Start();
	mTerrain->CreateResources();
	t.Stop();
	Debug::Info("GenerateHeightmap() took: {}ms to calculate!", t.ElapsedMilliseconds());


}

void TerrainGenerator::Erode()
{
	static GLuint ssbo = {0};
		
	// get min and max value of heightmap for normalisation
	float minValue = *std::min_element(mHeightmap.begin(), mHeightmap.end());
	float maxValue = *std::max_element(mHeightmap.begin(), mHeightmap.end());

	// normalise height points
	for (int i = 0; i < mHeightmap.size(); i++)
	{
		mHeightmap[i] = 0.0f + (1.0f - 0.0f) * (mHeightmap[i] - minValue) / (maxValue - minValue);
	}

	static Ptr<Shader> shader = nullptr;

	if (!shader)
	{
		shader = CreatePtr<Shader>("shaders/terrain/Erosion.comp");
	}
	shader->Bind();

	if (ssbo == 0) 
	{
		glGenBuffers(1, &ssbo);
	}


	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mHeightmap.size() * sizeof(GLfloat), mHeightmap.data(), GL_DYNAMIC_COPY); //sizeof(data) only works for statically sized C/C++ arrays.

	int heightMapResolution = mDimensions.x;
	int particleCount = heightMapResolution * 150;
	int maxLifeTime = 120;
	float inertia = 0.03f;
	float sedimentCapacityFactor = 6.0f;
	float minSedimentCapacity = 0.0f;
	float erosionSpeed = 0.3f;
	float evaporateSpeed = 0.02f;
	float depositSpeed = 0.3f;
	float startSpeed = 2.0f;
	float startWater = 1.0f;

	shader->SetUniformInt("HeightMapResolution", heightMapResolution);
	shader->SetUniformInt("ErosionParticleCount", particleCount);
	shader->SetUniformInt("ErosionMaxLifeTime", maxLifeTime);
	shader->SetUniformFloat("ErosionInertia", inertia);
	shader->SetUniformFloat("ErosionParticleStartSpeed", startSpeed);
	shader->SetUniformFloat("ErosionParticleStartWater", startWater);
	shader->SetUniformFloat("ErosionSedimentCapacityFactor", sedimentCapacityFactor);
	shader->SetUniformFloat("ErosionMinSedimentCapacity", minSedimentCapacity);
	shader->SetUniformFloat("ErosionSpeed", erosionSpeed);
	shader->SetUniformFloat("ErosionEvaporateSpeed", evaporateSpeed);
	shader->SetUniformFloat("ErosionGravity", 9.81f);
	shader->SetUniformFloat("ErosionDepositSpeed", depositSpeed);

	GLuint64 startTime, stopTime;
	unsigned int queryID[2];

	glGenQueries(2, queryID);
	glQueryCounter(queryID[0], GL_TIMESTAMP);

	glDispatchCompute(particleCount / 32, 1, 1);

	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	GLfloat* ptr;
	ptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glQueryCounter(queryID[1], GL_TIMESTAMP);
	// wait until the results are available
	GLint stopTimerAvailable = 0;
	while (!stopTimerAvailable) {
		glGetQueryObjectiv(queryID[1],
			GL_QUERY_RESULT_AVAILABLE,
			&stopTimerAvailable);
	}
	// get query results
	glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
	glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &stopTime);

	Debug::CoreLog("Erosion Compute shader took {}ms", (stopTime - startTime) / 1000000.0);




	for (int i = 0; i < mDimensions.x * mDimensions.y; i++) {
		//Debug::CoreCritical("{}", ptr[i]);
		mHeightmap[i] = ptr[i];
	}

	// "denormalise" height points
	for (int i = 0; i < mDimensions.x; i++)
	{
		for (int j = 0; j < mDimensions.y; j++)
		{
			int it = j * mDimensions.x + i;

			mHeightmap[it] = minValue + (maxValue - minValue) * (mHeightmap[it] - 0.0f) / (1.0f - 0.0f);
		}
	}

	for (int z = 0; z < mDimensions.x; z++)
	{
		for (int x = 0; x < mDimensions.y; x++)
		{
			mTerrain->SetHeight(x, z, mHeightmap[z * mDimensions.x + x]);
		}
	}


	mTerrain->CalculateNormals();

	mTerrain->CreateResources();
}

void TerrainGenerator::GenerateRiverPath()
{
	/*
	top left = x - row - 1
	top mid = x - row
	top right = x - row + 1

	mid left = x - 1
	mid mid = x
	mid right = x + 1

	bot left = x + row - 1
	bot mid = x + row
	bot right = x + row + 1
	*/


	Vec2 startPoint{ 100, 100 };

	int row = mDimensions.x;
	
	int x = startPoint.y * mDimensions.x + startPoint.x;

	while (true)
	{
		//int topleft = x - row - 1;
		//int topmid = x - row;
		//int topright = x - row + 1;
		//
		//int midleft = x - 1;
		//int midmid = x;
		//int midright = x + 1;
		//
		//int botleft = x + row - 1;
		//int botmid = x + row;
		//int botright = x + row + 1;

		float heightX = mHeightmap[x];

		// Calculate heights of the four nodes of the droplet's cell
		std::array<float, 4> adjCells;
		adjCells[0] = mHeightmap[x];
		adjCells[1] = mHeightmap[x + 1];
		adjCells[2] = mHeightmap[x + row];
		adjCells[3] = mHeightmap[x + row + 1];

		float min = *std::min_element(adjCells.begin(), adjCells.end());
	}


	

}
