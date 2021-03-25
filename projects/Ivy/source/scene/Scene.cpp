#include "ivypch.h"
#include "Scene.h"


Ivy::Ptr<Ivy::Scene> Ivy::Scene::mInstance = nullptr;

Ivy::Scene::Scene()
{
	mCamera = CreatePtr<Camera>(Vec3(0.0f, 0.0f, 0.0f));
	
	mCSM = CreatePtr<ShadowRenderPass>(mCamera);
	
	AddDirectionalLight(
		3.0f,							 //intensity
		Vec3(-2.0f,  4.0f,   -1.0f),	 //direction
		Vec3( 0.05f, 0.0492f, 0.0439f),  //ambient
		Vec3( 1.0f,  0.984f,  0.878f),	 //diffuse
		Vec3( 0.5f,  0.492f,  0.439f));	 //specular

	mCSM->SetDirLight(mDirLight);

	glGenQueries(1, &mQuery);

}

Ivy::Scene::~Scene()
{

}

Ivy::Ptr<Ivy::Entity> Ivy::Scene::CreateEntity()
{
	Ptr<Entity> ent = CreatePtr<Entity>();
	ent->mIndex = mEntities.size();
	ent->mCamera = mCamera;
	ent->AddComponent(CreatePtr<Transform>());
	ent->AddComponent(CreatePtr<Material>());
	ent->OnCreate();
	Debug::CoreLog("Created entity with index {}", ent->mIndex);
	
	mEntities.push_back(ent);

	//SortEntitiesForAlpha();
	return ent;
}

void Ivy::Scene::SetSkybox(Vector<String> filepaths)
{
	//mSkyboxFilepaths = filepaths;
	mScenePass->SetupSkybox(filepaths);
}

void Ivy::Scene::SetSkybox(String right, String left, String top, String bottom, String back, String front)
{
	Vector<String> filenames;
	filenames.push_back(right);
	filenames.push_back(left);
	filenames.push_back(top);
	filenames.push_back(bottom);
	filenames.push_back(back);
	filenames.push_back(front);

	SetSkybox(filenames);
}

void Ivy::Scene::Update(float deltaTime)
{
	if(mFirstUpdate)
	{
		for(auto& e : mEntities)
		{
			if(e->IsActive())
			{
				e->OnStart();
				e->StartComponents();
			}
		}

		mFirstUpdate = false;
	}

	mImGuiHook->NotifyNewFrame();

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
	static float timer = 0;
	static float fps = 0;
	timer += deltaTime;
	if(timer >= 1.0f)
	{
		fps = 1 / deltaTime;
		timer = 0;
	}

	ImGui::Begin("Controls");
	ImGui::Text("Show/Hide mouse cursor = M");
	ImGui::Text("Move around scene      = WASD");
	ImGui::Text("look around scene      = Mouse movement");
	ImGui::Text("Toggle Fullscreen mode = F11");
	ImGui::Text("Exit Program           = ESC");
	ImGui::End();


	ImGui::Begin("Scene Settings!");
	ImGui::Text("FPS: %f", fps);
	float v[3];
	v[0] = mDirLight.direction.x;
	v[1] = mDirLight.direction.y;
	v[2] = mDirLight.direction.z;

	static bool wireframe = false;
	ImGui::Checkbox("Wireframe", &wireframe);

	if(wireframe)
	{
		// Turn on wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		// Turn off wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	static float dirLightIntensity = 1.0f;
	static Vec3 dirLightDiff = Vec3(1.0f);
	static Vec3 dirLightAmb  = Vec3(1.0f);

	static float fov = 60.0f;
	String fovLabel = "FOV";
	if(fov >= 100)
	{
		fovLabel = "FOV: Quake Pro";
	}


	if(ImGui::SliderFloat(fovLabel.c_str(), &fov, 30.0f, 120.0f))
	{
		mCamera->SetFOV(fov);
	}

	ImGui::Text("Lighting settings");
	static bool useIBL = true;
	static float iblStrength = 0.2f;
	if(ImGui::Checkbox("Use IBL", &useIBL) || ImGui::SliderFloat("IBL Strength", &iblStrength, 0, 1))
	{
		for(auto& e : mEntities)
		{
			for(auto& c : e->GetComponentsOfType<Material>())
			{
				if(c != nullptr)
				{
					c->UseIBL(useIBL);
					c->SetIblStrength(iblStrength);
				}
			}
		}
	}

	if(ImGui::SliderFloat3("Sun direction", v, -6.28319f, 6.28319f))
	{
		mDirLight.direction = (Vec3(v[0],v[1], v[2]));
	}

	if(ImGui::SliderFloat("Sun intensity", &dirLightIntensity, 0.0f, 100.0f))
	{
		mDirLight.intensity = dirLightIntensity;
	}


	if(ImGui::ColorEdit3("Sun Diffuse", &dirLightDiff.x))
	{
		mDirLight.diffuse = dirLightDiff;
	}
	if(ImGui::ColorEdit3("Sun Ambient", &dirLightAmb.x))
	{
		mDirLight.ambient = dirLightAmb;
	}

	ImGui::Spacing();

	static float lightIntensity = 1.0f;
	ImGui::SliderFloat("Light intensity", &lightIntensity, 0.0f, 100.0f);
	static Vec3 diff = Vec3(1.0f);
	static Vec3 spec = Vec3(1.0f);
	ImGui::ColorEdit3("Diffuse", &diff.x);
	ImGui::ColorEdit3("Specular", &spec.x);

	if(ImGui::Button("Add pointlight"))
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

void Ivy::Scene::Render(float deltaTime, Vec2 currentWindowSize)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glBeginQuery(GL_SAMPLES_PASSED, mQuery);

	// Shadow Pass
	mCSM->RenderShadows(currentWindowSize, mEntities);
	// Scene pass
	mScenePass->Render(currentWindowSize);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	mPostprocessPass->Render(currentWindowSize, deltaTime);

	if(mRenderGui)
	{
		mImGuiHook->Render();
	}

	//glEndQuery(mQuery);
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
