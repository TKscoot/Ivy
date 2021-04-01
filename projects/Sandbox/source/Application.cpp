#include "Application.h"

Application::Application()
{
	mEngine = CreatePtr<Engine>();
	mEngine->Initialize(1600, 900, "Ivy Sandbox v0.3.7");
	//mEngine->GetWindow()->SetWindowIcon("assets/textures/Misc/awesomeface.png");

	mScene = SceneManager::GetInstance()->CreateSceneAndSetActive("TestScene");
	mScene2 = SceneManager::GetInstance()->CreateScene("TestScene2");

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
	mScene2->SetSkybox(skyboxTextures);
}

void Application::SetupEntities()
{
	// Animated Pilot
	Ptr<Entity> pilotEntity = mScene2->CreateEntity();
	pilotEntity->AddComponent(CreatePtr<Mesh>(pilotEntity.get(), "assets/models/Pilot_LP_Animated.fbx"));
	pilotEntity->GetFirstComponentOfType<Transform>()->setScale(Vec3(0.5f, 0.5f, 0.5f));
	Vector<Ptr<Material>> pilotMaterials = pilotEntity->GetComponentsOfType<Material>();
	for(auto& pilotMat : pilotMaterials)
	{
		pilotMat->LoadTexture("assets/textures/Pilot_LP_Animated/Material.002_Base_Color.png", Material::TextureMapType::DIFFUSE);
		pilotMat->LoadTexture("assets/textures/Pilot_LP_Animated/Material.002_Normal_OpenGL.png", Material::TextureMapType::NORMAL);
		pilotMat->LoadTexture("assets/textures/Pilot_LP_Animated/Material.002_Metallic.png", Material::TextureMapType::METALLIC);
		pilotMat->LoadTexture("assets/textures/Pilot_LP_Animated/Material.002_Roughness.png", Material::TextureMapType::ROUGHNESS);
		//shadowTestMaterial->SetMetallic(0.1f);
		//shadowTestMaterial->SetRoughness(0.9f);
		pilotMat->UseIBL(false);
	}
	pilotEntity->SetActive(true);
	/*
	*/

	// Sponza scene
	Ptr<Entity> sponzaEntity = mScene->CreateEntity();
	sponzaEntity->AddComponent(CreatePtr<Mesh>(sponzaEntity.get(), "assets/models/sponza_pbr.obj"));
	//sponzaEntity->AddComponent(CreatePtr<Mesh>(sponzaEntity.get(), "assets/models/MetalRoughSpheres.gltf"));
	//auto mat = sponzaEntity->GetFirstComponentOfType<Material>();
	//auto transform = sponzaEntity->GetFirstComponentOfType<Transform>();
	//transform->setRotationX(180);
	//transform->setPositionX(50);
	//mat->UseIBL(false);
	//mat->LoadTexture("assets/textures/MetalRoughSpheres/Spheres_Metal.png", Material::TextureMapType::METALLIC);
	//mat->LoadTexture("assets/textures/MetalRoughSpheres/Spheres_Roughness.png", Material::TextureMapType::ROUGHNESS);
	sponzaEntity->GetFirstComponentOfType<Transform>()->setScale(0.025f, 0.025f, 0.025f);
	/*

	sponzaEntity->SetActive(true);


	// General testing Entity
	Ptr<Entity> towerEntity = Scene::GetScene()->CreateEntity();
	towerEntity->AddComponent(CreatePtr<Mesh>(towerEntity.get(), "assets/models/Cerberus.FBX"));
	Ptr<Material> towerMat = towerEntity->GetFirstComponentOfType<Material>();
	towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_A.tga", Material::TextureMapType::DIFFUSE);
	towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_N.tga", Material::TextureMapType::NORMAL);
	towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_M.tga", Material::TextureMapType::METALLIC);
	towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_R.tga", Material::TextureMapType::ROUGHNESS);
	//
	Ptr<Transform> towerTransform = towerEntity->GetFirstComponentOfType<Transform>();
	//towerTransform->setPosition(0.0f, 5.0f, -30.0f);
	towerTransform->setPosition(0.0f, 5.0f, -3.0f);
	towerTransform->setScale(0.05f, 0.05f, 0.05f);
	towerTransform->setRotation(-90.0f, 0.0f, 0.0f);


	//
	Ptr<Entity> sceneBaseEntity = Scene::GetScene()->CreateEntity();
	sceneBaseEntity->AddComponent(CreatePtr<Mesh>(sceneBaseEntity.get(), "assets/models/Scene_Base.obj"));
	sceneBaseMaterials = sceneBaseEntity->GetComponentsOfType<Material>();

	sceneBaseMaterials[1]->SetTextureTiling(Vec2(5.0f, 5.0f));
	sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Grass002_diffuse.png", Material::TextureMapType::DIFFUSE);
	sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Grass002_normal.png", Material::TextureMapType::NORMAL);
	sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Grass002_roughness.png", Material::TextureMapType::ROUGHNESS);

	sceneBaseMaterials[2]->SetTextureTiling(Vec2(5.0f, 5.0f));
	sceneBaseMaterials[2]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Base_Color.jpg", Material::TextureMapType::DIFFUSE);
	sceneBaseMaterials[2]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Normal.jpg", Material::TextureMapType::NORMAL);
	sceneBaseMaterials[2]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Metallic.jpg", Material::TextureMapType::METALLIC);
	sceneBaseMaterials[2]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Roughness.jpg", Material::TextureMapType::ROUGHNESS);

	mBouncingEntity = mScene->CreateEntity();
	mBouncingEntity->AddComponent(CreatePtr<Mesh>(mBouncingEntity.get(), "assets/models/bunny.obj"));
	mBouncingEntity->SetActive(false);
	*/


	Ptr<CameraTracker> cameraTracker = mScene->CreateEntity<CameraTracker>(mScene->GetCamera());
	cameraTracker->AddTrackingPoint(Vec3(-10.0f, 2.0f, -10.0f));
	cameraTracker->AddTrackingPoint(Vec3(0.0f, 2.0f, 0.0f));
	cameraTracker->AddTrackingPoint(Vec3(10.0f, 2.0f, -20.0f));
	cameraTracker->ShouldPlay(false);

	//Ptr<AudioClip> soundClip = CreatePtr<AudioClip>(cameraTracker.get(), "assets/sounds/Polyphia - Nasty.flac", false);
	//cameraTracker->AddComponent<AudioClip>(soundClip);
	//soundClip->Play();

	//Ptr<TerrainGenerator> terrain = mScene2->CreateEntity<TerrainGenerator>();
}

void Application::Run()
{

	// vars for cursor visibility toggling
	bool cursorVisible = Input::GetMouseCursorVisible();
	bool cursorKeyPressed = false;

	bool setFullscreen = !mEngine->GetWindow()->IsFullscreen();

	float timer = 0;

	// Beginning the game loop
	while(!mEngine->ShouldTerminate())
	{
		timer += mEngine->GetDeltaTime();

		if(Input::IsMouseButtonDown(MouseCode::Button1))
		{
			SceneManager::GetInstance()->GetActiveScene()->GetCamera()->HandleInput(true);
			Input::SetMouseCursorVisible(false);
		}
		else
		{
			SceneManager::GetInstance()->GetActiveScene()->GetCamera()->HandleInput(false);
			Input::SetMouseCursorVisible(true);
		}
		
		if(Input::IsKeyDown(KeyCode::Escape))
		{
			exit(0);
		}

		if(Input::IsKeyDown(KeyCode::D2))
		{
			SceneManager::GetInstance()->SetActiveScene("TestScene2");
		}

		if(Input::IsKeyDown(KeyCode::D1))
		{
			SceneManager::GetInstance()->SetActiveScene("TestScene");
		}

		if(Input::IsKeyDown(M))
		{
			Input::SetMousePositon({mEngine->GetWindow()->GetWindowSize().x / 2.0f, mEngine->GetWindow()->GetWindowSize().y / 2.0f });
			mScene->GetCamera()->HandleInput(cursorVisible);
			Input::SetMouseCursorVisible(!cursorVisible);
			cursorVisible = !cursorVisible;
		}

		if(Input::IsKeyDown(F11))
		{
			mEngine->GetWindow()->SetFullscreen(setFullscreen);
			setFullscreen = !setFullscreen;
		}

		// Begin new frame
		mEngine->NewFrame();
	}
}
