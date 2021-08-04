#include "ivypch.h"
#include "Scene.h"


Ivy::Ptr<Ivy::Scene> Ivy::Scene::mInstance = nullptr;

Ivy::Scene::Scene(String name)
{
	mName = name;
	mCamera = CreatePtr<Camera>(Vec3(0.0f, 0.0f, 0.0f));
	
	mCSM = CreatePtr<ShadowRenderPass>(mCamera);
	
	AddDirectionalLight(
		3.0f,							 //intensity
		//Vec3(-2.0f,  4.0f,   -1.0f),	 //direction
		glm::vec3(0.0f, sin(35), cos(35)), 
		Vec3( 0.05f, 0.0492f, 0.0439f),  //ambient
		Vec3( 1.0f,  0.984f,  0.878f),	 //diffuse
		Vec3( 0.5f,  0.492f,  0.439f));	 //specular

	mCSM->SetDirLight(mDirLight);

	mSkyModel = CreatePtr<HosekWilkieSkyModel>();

}

Ivy::Scene::~Scene()
{

}

Ivy::Ptr<Ivy::Entity> Ivy::Scene::CreateEntity()
{
	Ptr<Entity> ent = CreatePtr<Entity>();
	ent->mIndex = mEntities.size();
	ent->mCamera = mCamera;
	ent->AddComponent<Transform>();
	ent->AddComponent<Material>();
	ent->OnCreate();
	ent->mScene = shared_from_this();
	ent->mAttachedSceneIsActive = mIsActiveScene;
	Debug::CoreLog("Created entity with index {} in scene {}", ent->mIndex, mName);
	
	mEntities.push_back(ent);

	//SortEntitiesForAlpha();
	return ent;
}

void Ivy::Scene::SetSkybox(Vector<String> filepaths)
{
	mSkyboxPaths = filepaths;
	mSkyboxType = CUBEMAP;
	mScenePass->SetEnvironmentMap(CreatePtr<TextureCube>(mSkyboxPaths));
}

void Ivy::Scene::SetSkybox(String right, String left, String top, String bottom, String back, String front)
{
	mSkyboxType = CUBEMAP;

	mSkyboxPaths.push_back(right);
	mSkyboxPaths.push_back(left);
	mSkyboxPaths.push_back(top);
	mSkyboxPaths.push_back(bottom);
	mSkyboxPaths.push_back(back);
	mSkyboxPaths.push_back(front);

	mUseSkybox = true;

	SetSkybox(mSkyboxPaths);
}

void Ivy::Scene::SetHdriEnvironment(String path)
{
	mSkyboxType = HDRI_MAP;

	mEnvMapPath = path;
	mUseSkybox = false;
	mScenePass->SetEnvironmentMap(mEnvMapPath);
}

void Ivy::Scene::SetHosekWilkieSkyModel(float turbidity)
{
	mSkyboxType = HOSEK_WILKIE_SKY;
	mUseSkybox = false;

	mSkyModel->SetTurbidity(turbidity);

	mScenePass->SetupSkyModel();
}

void Ivy::Scene::SetSunTime(float timeOfDay, float timeZone, int julianDay, float latitude, float longitude)
{
	float solarTime = timeOfDay
		+ (0.170f * glm::sin(4 * glm::pi<float>() * (julianDay - 80) / 373)
			- 0.129f * glm::sin(2 * glm::pi<float>() * (julianDay - 8) / 355))
		+ (longitude / 15 - timeZone);

	float solarDeclination = (0.4093f * glm::sin(2 * glm::pi<float>() * (julianDay - 81) / 368));

	float latRads = glm::radians(latitude);

	float sx = glm::cos(solarDeclination)                 * glm::sin(glm::pi<float>() * solarTime / 12);
	float sy = glm::cos(latRads) * glm::sin(solarDeclination)
		+ glm::sin(latRads) * glm::cos(solarDeclination) * glm::cos(glm::pi<float>() * solarTime / 12);
	float sz = glm::sin(latRads) * glm::sin(solarDeclination)
		- glm::cos(latRads) * glm::cos(solarDeclination) * glm::cos(glm::pi<float>() * solarTime / 12);

	SetDirectionalLightDirection(Vec3(sx, sy, sz));
}

void Ivy::Scene::Update(float deltaTime)
{
	if(mFirstUpdate)
	{
		for(int i = 0; i < mEntities.size(); i++)
		{
			if(mEntities[i]->IsActive())
			{
				mEntities[i]->OnStart();
				mEntities[i]->StartComponents();
			}
		}

		float sunAngle = glm::radians(35.0f);
		float sunAngle1 = glm::radians(35.0f);
		mDirLight.direction = glm::vec3(0.0f, sin(sunAngle), cos(sunAngle));
		mDirLight.direction = glm::vec3(sin(sunAngle1), cos(sunAngle1), 0.0f);

		mFirstUpdate = false;
	}

	//mImGuiHook->NotifyNewFrame();

	mCamera->Update(deltaTime);

	for(auto& e : mEntities)
	{
		if(e->mActive)
		{
			e->OnUpdate(deltaTime);
			e->UpdateComponents(deltaTime);
		}
	}

	mCSM->Update();


	ImGui::Begin("Controls");
	ImGui::Text("Show/Hide mouse cursor = M");
	ImGui::Text("Move around scene      = WASD");
	ImGui::Text("look around scene      = Mouse movement");
	ImGui::Text("Toggle Fullscreen mode = F11");
	ImGui::Text("Exit Program           = ESC");
	ImGui::End();

	DrawSceneSettingsGUI(deltaTime);
	
}

void Ivy::Scene::Render(float deltaTime, Vec2 currentWindowSize)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shadow pass
	mCSM->RenderShadows(currentWindowSize, mEntities);

	// Scene pass
	mScenePass->Render(deltaTime, currentWindowSize, mSkyboxType == HOSEK_WILKIE_SKY);

	// Postprocess pass
	mPostprocessPass->Render(currentWindowSize, deltaTime);

}

Ivy::DirectionalLight& Ivy::Scene::AddDirectionalLight(DirectionalLight lightParams)
{
	mDirLight = lightParams;
	return mDirLight;
}

Ivy::DirectionalLight& Ivy::Scene::AddDirectionalLight(
	float intensity, Vec3 direction, Vec3 ambient, Vec3 diffuse, Vec3 specular)
{
	DirectionalLight l;
	l.intensity = intensity;
	l.direction = direction;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;

	mDirLight = l;

	return mDirLight;
}

Ivy::PointLight& Ivy::Scene::AddPointLight(PointLight lightParams)
{
	mPointLights.push_back(lightParams);

	return lightParams;
}

Ivy::PointLight& Ivy::Scene::AddPointLight(
	Vec3 position, float constant, float linear,
	float quadratic, Vec3 ambient, Vec3 diffuse, Vec3 specular)
{
	PointLight l;
	l.position = position;
	l.constant = constant;
	l.linear = linear;
	l.quadratic = quadratic;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;

	uint32_t index = mPointLights.size();
	mPointLights.push_back(l);

	return mPointLights[index];
}

Ivy::SpotLight& Ivy::Scene::AddSpotLight(SpotLight& lightParams)
{
	mSpotLights.push_back(lightParams);

	return lightParams;
}

Ivy::SpotLight& Ivy::Scene::AddSpotLight(
	Vec3 position, Vec3 direction, float cutOff,
	float outerCutOff, float constant, float linear,
	float quadratic, Vec3 ambient, Vec3 diffuse, Vec3 specular)
{
	SpotLight l;
	l.position = position;
	l.direction = direction;
	l.cutOff = cutOff;
	l.outerCutOff = outerCutOff;
	l.constant = constant;
	l.linear = linear;
	l.quadratic = quadratic;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;

	uint32_t index = mSpotLights.size();
	mSpotLights.push_back(l);

	return mSpotLights[index];
}

void Ivy::Scene::Load()
{
	switch (mSkyboxType)
	{
	case Ivy::Scene::CUBEMAP:
		if (!mSkyboxPaths.empty())
		{
			mScenePass->SetEnvironmentMap(CreatePtr<TextureCube>(mSkyboxPaths));
		}
		break;
	case Ivy::Scene::HOSEK_WILKIE_SKY:
		mScenePass->SetupSkyModel();
		break;
	case Ivy::Scene::HDRI_MAP:
		if (!mEnvMapPath.empty())
		{
			mScenePass->SetEnvironmentMap(mEnvMapPath);
		}
		break;
	default:
		break;
	}


	for(auto& e : mEntities)
	{
		for(auto& m : e->mComponents)
		{
			auto components = m.second;
			for(auto& c : components)
			{
				if(c)
				{
					c->OnSceneLoad(); // Load components such as meshes for resource management
				}
			}
		}
		Debug::CoreLog("Loaded Entity {} in Scene: {}!", e->mIndex, mName);
	}

	Debug::CoreInfo("Scene load complete: {}!", mName);
}

void Ivy::Scene::Unload()
{
	for(auto& e : mEntities)
	{
		for(auto& m : e->mComponents)
		{
			auto components = m.second;
			for(auto& c : components)
			{
				if(c)
				{
					c->OnSceneUnload();
				}
			}
		}
	}

	mScenePass->UnloadEnvironmentMap();
	mScenePass->DestroySkybox();

}

void Ivy::Scene::DrawSceneSettingsGUI(float deltaTime)
{
	static float timer = 0;
	static float fps = 0;
	float frametime = deltaTime * 1000;

	timer += deltaTime;
	if (timer >= 1.0f)
	{
		fps = 1 / deltaTime;
		timer = 0;
	}

	ImGui::Begin("Scene Settings!");
	ImGui::Text("FPS: %.2f Frametime: %.2fms", fps, frametime);


	float v[3];
	v[0] = mDirLight.direction.x;
	v[1] = mDirLight.direction.y;
	v[2] = mDirLight.direction.z;

	static bool wireframe = false;
	;

	if (ImGui::Checkbox("Wireframe", &wireframe))
	{
		if (wireframe)
		{
			// Turn on wireframe mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			// Turn off wireframe mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}


	static float dirLightIntensity = 1.0f;
	static Vec3 dirLightDiff = Vec3(1.0f);
	static Vec3 dirLightAmb = Vec3(1.0f);

	static float fov = 60.0f;
	String fovLabel = "FOV";
	if (fov >= 100)
	{
		fovLabel = "FOV: Quake Pro";
	}


	if (ImGui::SliderFloat(fovLabel.c_str(), &fov, 30.0f, 120.0f))
	{
		mCamera->SetFOV(fov);
	}

	ImGui::Text("Lighting settings");
	static bool useIBL = true;
	static float iblStrength = 0.2f;
	if (ImGui::Checkbox("Use IBL", &useIBL) || ImGui::SliderFloat("IBL Strength", &iblStrength, 0, 1))
	{
		for (auto& e : mEntities)
		{
			for (auto& c : e->GetComponentsOfType<Material>())
			{
				if (c != nullptr)
				{
					c->UseIBL(useIBL);
					c->SetIblStrength(iblStrength);
				}
			}
		}
	}

	static float sunAngle = glm::radians(35.0f);
	static float sunAngle1 = glm::radians(35.0f);

	if (ImGui::SliderAngle("Sun direction", &sunAngle, 0.0f, 180.0f))
	{
		mDirLight.direction = glm::vec3(0.0f, sin(sunAngle), cos(sunAngle));

		if (mSkyboxType == HOSEK_WILKIE_SKY)
		{
			mScenePass->RenderSkyModelToCubemap();
			if (mRecalculateEnvMap)
			{
				mScenePass->ComputeEnvironmentMap();
			}
		}
	}
	if (ImGui::SliderFloat3("Sun dir vector", v, -2.6f, 2.6f))
	{
		mDirLight.direction = glm::vec3(v[0], v[1], v[2]);

		if (mSkyboxType == HOSEK_WILKIE_SKY)
		{
			mScenePass->RenderSkyModelToCubemap();
			if (mRecalculateEnvMap)
			{
				mScenePass->ComputeEnvironmentMap();
			}
		}
	}

	if (ImGui::SliderFloat("Sun intensity", &dirLightIntensity, 0.0f, 100.0f))
	{
		mDirLight.intensity = dirLightIntensity;
	}

	ImGui::Spacing();
	auto skyTypeName = magic_enum::enum_name(mSkyboxType);
	ImGui::Text("Sky type: %s", skyTypeName.data());


	switch (mSkyboxType)
	{
	case Ivy::Scene::NONE:
		break;
	case Ivy::Scene::CUBEMAP:
		break;
	case Ivy::Scene::HOSEK_WILKIE_SKY:
		DrawHosekWilkieSkyboxGUI(deltaTime);
		break;
	case Ivy::Scene::HDRI_MAP:
		break;
	default:
		break;
	}

	ImGui::Spacing();

	static float lightIntensity = 1.0f;
	ImGui::SliderFloat("Light intensity", &lightIntensity, 0.0f, 100.0f);
	static Vec3 diff = Vec3(1.0f);
	static Vec3 spec = Vec3(1.0f);
	ImGui::ColorEdit3("Diffuse", &diff.x);
	ImGui::ColorEdit3("Specular", &spec.x);

	if (ImGui::Button("Add pointlight"))
	{
		PointLight& pl = AddPointLight(
			mCamera->GetPosition(),
			lightIntensity,
			0.09f,
			0.032,
			Vec3(0.05f, 0.05f, 0.05f),
			diff,
			spec);
	}

	ImGui::End();
}

void Ivy::Scene::DrawHosekWilkieSkyboxGUI(float deltaTime)
{
	static float turbidity = 4.0f;
	static bool animateSky = false;

	static float skyTimer = 0.0f;
	static float skyTimerMultiplier = 0.1f;

	ImGui::SliderFloat("Sky timer multiplier", &skyTimerMultiplier, 0.0f, 2.0f);


	ImGui::Checkbox("Recalculate env map", &mRecalculateEnvMap);
	ImGui::Checkbox("Animate sky", &animateSky);

	if (animateSky)
	{
		skyTimer += deltaTime * skyTimerMultiplier;

		if (glm::degrees(skyTimer) < 180.0f)
		{
			mDirLight.direction = glm::vec3(0.0f, sin(skyTimer), cos(skyTimer));
			mScenePass->RenderSkyModelToCubemap();
			if (mRecalculateEnvMap)
			{
				mScenePass->ComputeEnvironmentMap();
			}
		}
		else if (glm::degrees(skyTimer) >= 180.0f)
		{
			skyTimer = 0.0f;
		}
	}



	if (ImGui::SliderFloat("Turbidity", &turbidity, 0.0f, 10.0f))
	{
		mSkyModel->SetTurbidity(turbidity);
	}

	ImGui::SliderFloat("Cloud cirrus", &mScenePass->GetCloudsData().cirrus, 0.0f, 10.0f);
	ImGui::SliderFloat("Cloud cumulus", &mScenePass->GetCloudsData().cumulus, 0.0f, 10.0f);
}
