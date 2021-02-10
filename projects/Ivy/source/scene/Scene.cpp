#include "ivypch.h"
#include "Scene.h"


Ivy::Ptr<Ivy::Scene> Ivy::Scene::mInstance = nullptr;

Ivy::Scene::Scene()
{
	mCamera = CreatePtr<Camera>(Vec3(0.0f, 0.0f, 0.0f));
	
	mCSM = CreatePtr<ShadowRenderPass>(mCamera);
	
	AddDirectionalLight(
		Vec3(-2.0f,  4.0f,   -1.0f),	 //direction
		Vec3( 0.05f, 0.0492f, 0.0439f),  //ambient
		Vec3( 1.0f,  0.984f,  0.878f),	 //diffuse
		Vec3( 0.5f,  0.492f,  0.439f));	 //specular

	mCSM->SetDirLight(mDirLight);
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


	ImGui::Begin("Scene Settings!");
	ImGui::Text("FPS: %f", fps);
	float v[3];
	v[0] = mDirLight.direction.x;
	v[1] = mDirLight.direction.y;
	v[2] = mDirLight.direction.z;


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

	if(ImGui::SliderFloat3("Sun direction", v, -6.28319f, 6.28319f))
	{
		mDirLight.direction = (Vec3(v[0],v[1], v[2]));
		mCSM->SetDirLight(mDirLight);
	}
	ImGui::End();
}

void Ivy::Scene::Render(float deltaTime, Vec2 currentWindowSize)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shadow Pass
	mCSM->RenderShadows(currentWindowSize, mEntities);
	// Scene pass
	mScenePass->Render(currentWindowSize);

	mPostprocessPass->Render(currentWindowSize, deltaTime);

	if(mRenderGui)
	{
		mImGuiHook->Render();
	}
}

Ivy::DirectionalLight& Ivy::Scene::AddDirectionalLight(DirectionalLight lightParams)
{
	mDirLight = lightParams;
	return mDirLight;
}

Ivy::DirectionalLight& Ivy::Scene::AddDirectionalLight(
	Vec3 direction, Vec3 ambient, Vec3 diffuse, Vec3 specular)
{
	DirectionalLight l;
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
