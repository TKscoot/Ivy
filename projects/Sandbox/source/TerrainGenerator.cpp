#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator()
{
	mDimensions = { 1024, 1024 };
}

void TerrainGenerator::OnCreate()
{
	mTerrain = mScene->CreateEntity<Terrain>(mDimensions.x, mDimensions.y);
	//mTerrain->GetComponent<Transform>()->setPositionX(80.0f);
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
		auto t = ent->GetComponent<Transform>();

		float h = mTerrain->GetHeightFromWorldPos(Vec3(i * 5, 0.0f, 0.0f));

		Debug::Log("Height: {}", h);
		
		t->setPosition(Vec3(i * 5, h, 0));
	}


}

void TerrainGenerator::OnUpdate(float deltaTime)
{
	ImGui::Begin("Terrain");
	ImGui::SliderFloat("Steepness cutoff", &mSteepnessCutoff, 0.0, 1.0);

	if (ImGui::Button("Generate")) {
		GenerateHeightmap();
	}

	if(ImGui::CollapsingHeader("Erosion"))
	{
		ImGui::BeginChild("Erosion", ImVec2(0, 420), true);

		if(ImGui::Button("Erode!"))
		{
			Erode();
		}
		ImGui::Spacing();

		ImGui::EndChild();
	}
	ImGui::Spacing();
	ImGui::End();


	static bool wireframe = false;
	if (Input::IsKeyDown(F3))
	{
		Ptr<Shader> wireframeShader = nullptr;
		if (wireframe)
		{
			mTerrain->GetMaterial()->InitDefaultShader();
			wireframe = false;
		}
		else
		{
			wireframeShader = CreatePtr<Shader>(
				"shaders/debug/Wireframe_Terrain.vert",
				"shaders/debug/Terrain_Wireframe.frag",
				"shaders/debug/Wireframe.geom");
			
			auto& tm = mTerrain->GetComponents<TerrainMaterial>();
			if (!tm.empty())
			{
				for (auto& mat : tm)
				{
					mat->SetShader(wireframeShader);
				}
			}

			wireframe = true;
		}
	}
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
			for(int octave = 0; octave < 4; octave++)
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
