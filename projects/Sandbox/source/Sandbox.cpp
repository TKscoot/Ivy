#include <Ivy.h>
#include <chrono>
#include <string>
#include <sstream>
#include <GLFW/glfw3.h>

#include "CameraTracker.h"

int main()
{
	using namespace Ivy;

	std::unique_ptr<Engine> engine = std::make_unique<Engine>();
	engine->Initialize(1280, 720, "Ivy Sandbox v0.0.5");

	//Vector<String> skyboxTextures =
	//{
	//	"assets/textures/skybox/Default/sky8_RT.jpg",
	//	"assets/textures/skybox/Default/sky8_LF.jpg",
	//	"assets/textures/skybox/Default/sky8_UP.jpg",
	//	"assets/textures/skybox/Default/sky8_DN.jpg",
	//	"assets/textures/skybox/Default/sky8_BK.jpg",
	//	"assets/textures/skybox/Default/sky8_FR.jpg",
	//};

	Vector<String> skyboxTextures =
	{
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};
	Scene::GetScene()->SetSkybox(skyboxTextures);

	/*
	Scene::GetScene()->SetDirectionalLightDirection(Vec3(0.5f, 2.0f, 2.0f));
	*/
	Scene::GetScene()->AddPointLight(
		Vec3(3.0f, 1.0f, 0.0f),
		1.0f,
		0.09f,
		0.032,
		Vec3(0.05f, 0.05f, 0.05f),
		Vec3(0.8f, 0.8f, 0.8f),
		Vec3(1.0f, 1.0f, 1.0f));


	// Object to test shadowmap
	Ptr<Entity> shadowTestEntity = Scene::GetScene()->CreateEntity();
	shadowTestEntity->AddComponent(CreatePtr<Mesh>(shadowTestEntity.get(), "assets/models/ShadowTest.obj"));
	Vector<Ptr<Material>> shadowTestMaterials = shadowTestEntity->GetComponentsOfType<Material>();
	for(auto& shadowTestMaterial : shadowTestMaterials)
	{
		//shadowTestMaterial->LoadTexture("assets/textures/Misc/box.jpg", Material::TextureMapType::DIFFUSE);
		//shadowTestMaterial->LoadTexture("assets/textures/sponza/spnza_bricks_a_ddn.png", Material::TextureMapType::NORMAL);
		//shadowTestMaterial->LoadTexture("assets/textures/Cerberus/Cerberus_M.tga", Material::TextureMapType::METALLIC);
		//shadowTestMaterial->LoadTexture("assets/textures/Survival_BackPack_2/1001_roughness.jpg", Material::TextureMapType::ROUGHNESS);
	}
	//shadowTestEntity->GetFirstComponentOfType<Transform>()->setScale(0.025f, 0.025f, 0.025f);
	shadowTestEntity->GetFirstComponentOfType<Transform>()->setScale(20.0f, 20.0f, 20.0f);
	//shadowTestEntity->GetFirstComponentOfType<Transform>()->setPosition(100.0f, 0.0f, 0.0f); 

	// General testing Entity
	Ptr<Entity> towerEntity = Scene::GetScene()->CreateEntity();
	towerEntity->AddComponent(CreatePtr<Mesh>(towerEntity.get(), "assets/models/Cerberus.FBX"));
	Ptr<Material> towerMat = towerEntity->GetFirstComponentOfType<Material>();
	towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_N.tga", Material::TextureMapType::NORMAL);
	towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_M.tga", Material::TextureMapType::METALLIC);
	towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_R.tga", Material::TextureMapType::ROUGHNESS);
	
	Ptr<Transform> towerTransform = towerEntity->GetFirstComponentOfType<Transform>();
	towerTransform->setPosition(1.0f, 1.0f, 0.0f);
	//towerTransform->setScale(100.0f, 100.0f, 100.0f); 
	towerTransform->setScale(0.025f, 0.025f, 0.025f);
	towerTransform->setRotation(-90.0f, 0.0f, 0.0f);

	Ptr<CameraTracker> cameraTracker = Scene::GetScene()->CreateEntity<CameraTracker>(Scene::GetScene()->GetCamera());
	cameraTracker->AddTrackingPoint(Vec3(-10.0f, 2.0f, -10.0f));
	cameraTracker->AddTrackingPoint(Vec3(  0.0f, 2.0f, -5.0f));
	cameraTracker->AddTrackingPoint(Vec3( 10.0f, 2.0f, -20.0f));
	cameraTracker->ShouldPlay(false);
	float timer = 0.0f;
	while(!engine->ShouldTerminate())
	{
		if(Input::IsKeyDown(KeyCode::Escape))
		{
			exit(0);
		}
		

		//if(timer <= 10.0)
		//{
		//	timer += engine->GetDeltaTime();
		//	Vec3 lightPos = Vec3(0.0f, glm::radians(glm::pi<float>() * 2 * timer), 0.0f);
		//	Debug::Log(lightPos.y);
		//	Scene::GetScene()->SetDirectionalLightDirection(lightPos);
		//}
		//else
		//{
		//	timer = 0;
		//}

		engine->NewFrame();
		
	}

	return 0;
}
