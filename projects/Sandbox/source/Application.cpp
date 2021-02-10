#include "Application.h"

Application::Application()
{
	mEngine = CreatePtr<Engine>();
	mEngine->Initialize(1600, 900, "Ivy Sandbox v0.3.7");
	//mEngine->GetWindow()->SetWindowIcon("assets/textures/Misc/awesomeface.png");

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
	/*
	mScene->AddPointLight(
		Vec3(25.0f, 5.0f, 30.0f),
		64.0f,
		0.09f,
		0.032f,
		Vec3(0.05f, 0.05f, 0.05f),
		Vec3(0.8f, 0.55f, 0.55f),
		Vec3(1.0f, 1.0f, 1.0f));
	mScene->AddPointLight(
		Vec3(25.0f, 5.0f, 0.0f),
		64.0f,
		0.09f,
		0.032f,
		Vec3(0.05f, 0.05f, 0.05f),
		Vec3(0.65f, 0.9f, 0.65f),
		Vec3(1.0f, 1.0f, 1.0f));

	mScene->AddPointLight(
		Vec3(25.0f, 5.0f, -30.0f),
		64.0f,
		0.09f,
		0.032f,
		Vec3(0.05f, 0.05f, 0.05f),
		Vec3(0.48f, 0.52f, 0.83f),
		Vec3(1.0f, 1.0f, 1.0f));
	*/



}

void Application::SetupEntities()
{
	// Animated Pilot
	Ptr<Entity> pilotEntity = Scene::GetScene()->CreateEntity();
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
		//pilotMat->UseIBL(false);
	}
	pilotEntity->SetActive(true);
	
	
	// Sponza scene
	Ptr<Entity> sponzaEntity = Scene::GetScene()->CreateEntity();
	sponzaEntity->AddComponent(CreatePtr<Mesh>(sponzaEntity.get(), "assets/models/Scene_Base.obj"));
	//sponzaEntity->GetFirstComponentOfType<Transform>()->setScale(0.025f, 0.025f, 0.025f);
	//for(auto& mat : sponzaEntity->GetComponentsOfType<Material>())
	//{
	//	mat->LoadTexture("assets/textures/Concrete_Wall/2K-concrete_48_Base Color.jpg", Material::TextureMapType::DIFFUSE);
	//	//mat->LoadTexture("assets/textures/Concrete_Wall/2K-concrete_48_Normal.jpg", Material::TextureMapType::NORMAL);
	//	mat->LoadTexture("assets/textures/Concrete_Wall/2K-concrete_48_Metallic.jpg", Material::TextureMapType::METALLIC);
	//	//mat->LoadTexture("assets/textures/Concrete_Wall/2K-concrete_48_Roughness.jpg", Material::TextureMapType::ROUGHNESS);
	//	mat->UseIBL(false);
	//	//mat->SetMetallic(0.001f);
	//	mat->SetTextureTiling(Vec2(4.0f));
	//}
	sponzaEntity->SetActive(true);
	/*


	
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
	towerTransform->setPosition(0.0f, 5.0f, -30.0f);
	towerTransform->setScale(0.05f, 0.05f, 0.05f);
	towerTransform->setRotation(-90.0f, 0.0f, 0.0f);
	//
	//Ptr<Entity> sceneBaseEntity = Scene::GetScene()->CreateEntity();
	//sceneBaseEntity->AddComponent(CreatePtr<Mesh>(sceneBaseEntity.get(), "assets/models/Scene_Base.obj"));
	//Vector<Ptr<Material>> sceneBaseMaterials = sceneBaseEntity->GetComponentsOfType<Material>();
	//
	//sceneBaseMaterials[1]->SetTextureTiling(Vec2(5.0f, 5.0f));
	//sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Grass002_diffuse.png", Material::TextureMapType::DIFFUSE);
	//sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Grass002_normal.png", Material::TextureMapType::NORMAL);
	//sceneBaseMaterials[1]->LoadTexture("assets/textures/Scene_Base/Grass002_roughness.png", Material::TextureMapType::ROUGHNESS);
	//
	//sceneBaseMaterials[2]->SetTextureTiling(Vec2(5.0f, 5.0f));
	//sceneBaseMaterials[2]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Base_Color.jpg", Material::TextureMapType::DIFFUSE);
	//sceneBaseMaterials[2]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Normal.jpg", Material::TextureMapType::NORMAL);
	//sceneBaseMaterials[2]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Metallic.jpg", Material::TextureMapType::METALLIC);
	//sceneBaseMaterials[2]->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Roughness.jpg", Material::TextureMapType::ROUGHNESS);

	mBouncingEntity = mScene->CreateEntity();
	mBouncingEntity->AddComponent(CreatePtr<Mesh>(mBouncingEntity.get(), "assets/models/bunny.obj"));


	*/
	Ptr<CameraTracker> cameraTracker = Scene::GetScene()->CreateEntity<CameraTracker>(Scene::GetScene()->GetCamera());
	cameraTracker->AddTrackingPoint(Vec3(-10.0f, 2.0f, -10.0f));
	cameraTracker->AddTrackingPoint(Vec3(0.0f, 2.0f, -5.0f));
	cameraTracker->AddTrackingPoint(Vec3(10.0f, 2.0f, -20.0f));
	cameraTracker->ShouldPlay(false);


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
		//Ptr<Transform> bouncingEntTransform = mBouncingEntity->GetFirstComponentOfType<Transform>();
		//bouncingEntTransform->setRotation(0.0f, timer * 50.0f, 0.0f);
		//bouncingEntTransform->setPosition(0.0f, 5.0f + glm::sin(timer), 30.0f);

		if(Input::IsKeyBeingPressed(KeyCode::Escape))
		{
			exit(0);
		}

		if(Input::IsKeyBeingPressed(KeyCode::R))
		{
			mScene->SortEntitiesForAlpha();
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
