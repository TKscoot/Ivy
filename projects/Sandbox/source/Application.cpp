#include "Application.h"

Application::Application()
{
	mEngine = CreatePtr<Engine>();
	mEngine->Initialize(1280, 720, "Ivy Sandbox v0.3.7");

	mEngine->GetWindow()->SetWindowIcon("assets/textures/Misc/awesomeface.png");

	mScene = Scene::GetScene();

	SetupScene();
	SetupEntities();
}

void Application::SetupScene()
{
	// Setting up skybox with 6 textures for the cubetexture
	Vector<String> skyboxTextures =
	{
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};
	mScene->SetSkybox(skyboxTextures);
	// Adding lights to the scene
	//mScene->AddPointLight(
	//	Vec3(-1.0f, 1.0f, 0.0f),
	//	1.0f,
	//	0.09f,
	//	0.032,
	//	Vec3(0.05f, 0.05f, 0.05f),
	//	Vec3(1.0f, 0.0f, 0.0f),
	//	Vec3(1.0f, 1.0f, 1.0f));
	//mScene->AddPointLight(
	//	Vec3(1.0f, 1.0f, 0.0f),
	//	1.0f,
	//	0.09f,
	//	0.032,
	//	Vec3(0.05f, 0.05f, 0.05f),
	//	Vec3(0.0f, 1.0f, 0.0f),
	//	Vec3(1.0f, 1.0f, 1.0f));
	//mScene->AddPointLight(
	//	Vec3(0.0f, 1.0f, 1.0f),
	//	1.0f,
	//	0.09f,
	//	0.032,
	//	Vec3(0.05f, 0.05f, 0.05f),
	//	Vec3(0.0f, 0.0f, 1.0f),
	//	Vec3(1.0f, 1.0f, 1.0f));
}

void Application::SetupEntities()
{
	// Object to test shadowmap
	//Ptr<Entity> shadowTestEntity = Scene::GetScene()->CreateEntity();
	//shadowTestEntity->AddComponent(CreatePtr<Mesh>(shadowTestEntity.get(), "assets/models/sponza_pbr.obj"));
	//Vector<Ptr<Material>> shadowTestMaterials = shadowTestEntity->GetComponentsOfType<Material>();
	//for(auto& shadowTestMaterial : shadowTestMaterials)
	//{
	//	//shadowTestMaterial->LoadTexture("assets/textures/Misc/box.jpg", Material::TextureMapType::DIFFUSE);
	//	//shadowTestMaterial->LoadTexture("assets/textures/Misc/brick_normal.jpg", Material::TextureMapType::NORMAL);
	//	//shadowTestMaterial->LoadTexture("assets/textures/Cerberus/Cerberus_M.tga", Material::TextureMapType::METALLIC);
	//	//shadowTestMaterial->LoadTexture("assets/textures/Survival_BackPack_2/1001_roughness.jpg", Material::TextureMapType::ROUGHNESS);
	//	shadowTestMaterial->SetMetallic(0.1f);
	//	shadowTestMaterial->SetRoughness(0.9f);
	//}
	//shadowTestEntity->GetFirstComponentOfType<Transform>()->setScale(0.025f, 0.025f, 0.025f);
	//shadowTestEntity->GetFirstComponentOfType<Transform>()->setScale(20.0f, 20.0f, 20.0f);
	//shadowTestEntity->GetFirstComponentOfType<Transform>()->setPosition(-2.0f, 4.0f, -3.0f);

	// General testing Entity
	//Ptr<Entity> towerEntity = Scene::GetScene()->CreateEntity();
	//towerEntity->AddComponent(CreatePtr<Mesh>(towerEntity.get(), "assets/models/Cerberus.FBX"));
	//Ptr<Material> towerMat = towerEntity->GetFirstComponentOfType<Material>();
	//towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_N.tga", Material::TextureMapType::NORMAL);
	//towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_M.tga", Material::TextureMapType::METALLIC);
	//towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_R.tga", Material::TextureMapType::ROUGHNESS);
	//
	//Ptr<Transform> towerTransform = towerEntity->GetFirstComponentOfType<Transform>();
	//towerTransform->setPosition(3.0f, 3.0f, 0.0f);
	////towerTransform->setScale(100.0f, 100.0f, 100.0f); 
	//towerTransform->setScale(0.05f, 0.05f, 0.05f);
	//towerTransform->setRotation(-90.0f, 0.0f, 0.0f);

	Ptr<Entity> sceneBaseEntity = Scene::GetScene()->CreateEntity();
	sceneBaseEntity->AddComponent(CreatePtr<Mesh>(sceneBaseEntity.get(), "assets/models/Pilot_LP_Animated.fbx"));
	//Vector<Ptr<Material>> sceneBaseMaterials = sceneBaseEntity->GetComponentsOfType<Material>();
	//
	//sceneBaseMaterials[0]->SetTextureTiling(Vec2(5.0f, 5.0f));
	//sceneBaseMaterials[0]->LoadTexture("assets/textures/Scene_Base/Grass002_diffuse.png", Material::TextureMapType::DIFFUSE);
	//sceneBaseMaterials[0]->LoadTexture("assets/textures/Scene_Base/Grass002_normal.png", Material::TextureMapType::NORMAL);
	//sceneBaseMaterials[0]->LoadTexture("assets/textures/Scene_Base/Grass002_roughness.png", Material::TextureMapType::ROUGHNESS);
	//
	//sceneBaseMaterials[1]->SetTextureTiling(Vec2(5.0f, 5.0f));
	//sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Base_Color.jpg", Material::TextureMapType::DIFFUSE);
	//sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Normal.jpg", Material::TextureMapType::NORMAL);
	//sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Metallic.jpg", Material::TextureMapType::METALLIC);
	//sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Roughness.jpg", Material::TextureMapType::ROUGHNESS);

	Ptr<CameraTracker> cameraTracker = Scene::GetScene()->CreateEntity<CameraTracker>(Scene::GetScene()->GetCamera());
	//cameraTracker->AddTrackingPoint(Vec3(-10.0f, 2.0f, -10.0f));
	//cameraTracker->AddTrackingPoint(Vec3(0.0f, 2.0f, -5.0f));
	//cameraTracker->AddTrackingPoint(Vec3(10.0f, 2.0f, -20.0f));
	cameraTracker->ShouldPlay(false);

	//Ptr<Entity> box1 = Scene::GetScene()->CreateEntity();
	//box1->AddComponent(CreatePtr<Mesh>(box1.get(), "assets/models/WoodBox.obj"));
	//Ptr<Material> box1Mat = box1->GetFirstComponentOfType<Material>();
	//Ptr<Transform> box1Trans = box1->GetFirstComponentOfType<Transform>();
	//
	//box1Mat->LoadTexture("assets/textures/Misc/awesomeface.png", Material::TextureMapType::DIFFUSE);
	//box1Trans->setPositionX(5.0f);
	//
	//Ptr<Entity> box2 = Scene::GetScene()->CreateEntity();
	//box2->AddComponent(CreatePtr<Mesh>(box2.get(), "assets/models/WoodBox.obj"));
	//Ptr<Material> box2Mat = box2->GetFirstComponentOfType<Material>();
	//Ptr<Transform> box2Trans = box2->GetFirstComponentOfType<Transform>();
	//box2Trans->setPositionX(-1.0f);
	//
	//box2Mat->LoadTexture("assets/textures/Misc/awesomeface.png", Material::TextureMapType::DIFFUSE);


}

void Application::Run()
{

	// vars for cursor visibility toggling
	bool cursorVisible = Input::GetMouseCursorVisible();
	bool cursorKeyPressed = false;

	// Beginning the game loop
	while(!mEngine->ShouldTerminate())
	{
		if(Input::IsKeyBeingPressed(KeyCode::Escape))
		{
			exit(0);
		}

		if(Input::IsKeyBeingPressed(KeyCode::R))
		{
			mScene->SortEntitiesForAlpha();
		}

		// Handle cursor toggling
		//bool cursorKeyCurrentlyPressed = Input::IsKeyPressed(M);

		//if(!cursorKeyPressed && cursorKeyCurrentlyPressed)
		//{ 
		//	mScene->GetCamera()->HandleInput(cursorVisible);
		//	Input::SetMouseCursorVisible(!cursorVisible);
		//	cursorVisible = !cursorVisible;
		//}
		//cursorKeyPressed = cursorKeyCurrentlyPressed;

		if(Input::IsKeyDown(M))
		{
			mScene->GetCamera()->HandleInput(cursorVisible);
			Input::SetMouseCursorVisible(!cursorVisible);
			cursorVisible = !cursorVisible;
		}



		// Begin new frame
		mEngine->NewFrame();

	}
}
