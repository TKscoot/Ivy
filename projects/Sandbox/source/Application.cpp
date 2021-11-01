#include "Application.h"
#include "core/BitMask.h"

Application::Application()
{
	TestBitmask();


	mEngine = CreatePtr<Engine>();
	mEngine->Initialize(1600, 900, "Ivy Sandbox v0.3.7");
	//mEngine->GetWindow()->SetWindowIcon("assets/textures/Misc/awesomeface.png");

	mScene = SceneManager::GetInstance()->CreateScene("TestScene");
	mScene2 = SceneManager::GetInstance()->CreateScene("SponzaScene");

	SetupScene();
	SetupEntities();

	SceneManager::GetInstance()->SetActiveScene("SponzaScene");


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
	//mScene2->SetSkybox(skyboxTextures);
	
	
	//mScene->SetHdriEnvironment("assets/env/HDR_041_Path.hdr");
	mScene2->SetHdriEnvironment("assets/env/HDR_041_Path.hdr");
	//mScene->UseHosekWilkieSkymodel();
	//mScene->SetHosekWilkieSkyModel();

}

void Application::SetupEntities()
{
	// Animated Pilot
	Ptr<Entity> pilotEntity = mScene2->CreateEntity();
	pilotEntity->AddComponent<Mesh>("assets/models/Scene_Base.obj");
	//pilotEntity->GetComponent<Transform>()->setScale(Vec3(0.5f, 0.5f, 0.5f));
	Vector<Ptr<Material>> pilotMaterials = pilotEntity->GetComponents<Material>();
	for(auto& pilotMat : pilotMaterials)
	{
		pilotMat->LoadTexture("assets/textures/Pilot_LP_Animated/Material.002_Base_Color.png", Material::TextureMapType::ALBEDO);
		pilotMat->LoadTexture("assets/textures/Scene_Base/Stone_rockWall_02_Normal.jpg", Material::TextureMapType::NORMAL);
		pilotMat->LoadTexture("assets/textures/Pilot_LP_Animated/Material.002_Metallic.png", Material::TextureMapType::METALLIC);
		pilotMat->LoadTexture("assets/textures/Pilot_LP_Animated/Material.002_Roughness.png", Material::TextureMapType::ROUGHNESS);
		//shadowTestMaterial->SetMetallic(0.1f);
		//shadowTestMaterial->SetRoughness(0.9f);
		pilotMat->UseIBL(false);
	}
	pilotEntity->SetActive(false);

	// Sponza scene
	Ptr<Entity> sponzaEntity = mScene2->CreateEntity(); 
	sponzaEntity->AddComponent<Mesh>("assets/models/sponza_pbr.obj");
	sponzaEntity->GetComponent<Transform>()->setScale(0.025f, 0.025f, 0.025f);
	/*
	sponzaEntity->SetActive(false);

	*/

	//planeEntity = mScene->CreateEntity();
	//
	//planeEntity->AddComponent<Mesh>("assets/models/piper_pa18.obj");
	//auto mat = planeEntity->GetComponent<Material>();
	//planeTransform = planeEntity->GetComponent<Transform>();
	////transform->setRotationX(90);
	//planeTransform->setPositionY(15.0f);
	////transform->setPositionX(50);
	////mat->UseIBL(false);
	//mat->LoadTexture("assets/textures/DamagedHelmet/Default_albedo.jpg", Material::TextureMapType::ALBEDO);
	//mat->LoadTexture("assets/textures/DamagedHelmet/Default_metalRoughness.jpg", Material::TextureMapType::METALLIC);
	//mat->LoadTexture("assets/textures/DamagedHelmet/Default_metalRoughness.jpg", Material::TextureMapType::ROUGHNESS);
	//mat->LoadTexture("assets/textures/DamagedHelmet/Default_normal.jpg", Material::TextureMapType::NORMAL);
	//
	//planeEntity->SetActive(false);

	/*

	sponzaEntity->AddComponent(CreatePtr<Mesh>(sponzaEntity.get(), "assets/models/MetalRoughSpheres.gltf"));
	auto mat = sponzaEntity->GetFirstComponentOfType<Material>();
	auto transform = sponzaEntity->GetFirstComponentOfType<Transform>();
	transform->setRotationX(180);
	//transform->setPositionX(50);
	mat->UseIBL(false);
	mat->LoadTexture("assets/textures/MetalRoughSpheres/Spheres_Metal.png", Material::TextureMapType::METALLIC);
	mat->LoadTexture("assets/textures/MetalRoughSpheres/Spheres_Roughness.png", Material::TextureMapType::ROUGHNESS);

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

	Ptr<TerrainGenerator> terrain = mScene->CreateEntity<TerrainGenerator>();
	//terrain->SetTerrainEntity(mScene->CreateEntity<Terrain>(1024, 1024));

	//Ptr<AudioTest> audioTest = mScene->CreateEntity<AudioTest>();


	//mScene->GetCamera()->GetTransform()->SetParent(sponzaTransform.get());
	//Vec3 camOffset = Vec3(0.0f, 4.0f, -10.0f);
	//SceneManager::GetInstance()->GetActiveScene()->GetCamera()->SetPosition(camOffset);

}

void Application::Run()
{

	// vars for cursor visibility toggling
	bool cursorVisible = Input::GetMouseCursorVisible();
	bool cursorKeyPressed = false;

	bool setFullscreen = !mEngine->GetWindow()->IsFullscreen();

	float timer = 0.0f;

	bool wireframe = false;

	// Beginning the game loop
	while(!mEngine->ShouldTerminate())
	{
		float dt = mEngine->GetDeltaTime();

		// Plane anim
		timer += dt * 0.001f;

		float speed = 5.0f;
		/*
		const Mat4 inverted = glm::inverse(planeTransform->getComposed());
		const Vec3 forward = normalize(glm::vec3(inverted[2]));

		planeTransform->setPosition((planeTransform->getPosition()) + (dt * planeTransform->getDirection() * speed));


		//SceneManager::GetInstance()->GetActiveScene()->GetCamera()->LookAt(sponzaTransform->getPosition() + Vec3(0.0f, 0.0f, 5.0f));
		//SceneManager::GetInstance()->GetActiveScene()->GetCamera()->SetFront(sponzaTransform->getDirection());


		if (Input::IsKeyBeingPressed(A)) 
		{
			planeTransform->setRotationY(planeTransform->getRotation().y + (timer * 10.0f));
		}
		if (Input::IsKeyBeingPressed(D))
		{
			planeTransform->setRotationY(planeTransform->getRotation().y - (timer * 10.0f));
		}

		if (Input::IsKeyBeingPressed(W))
		{
			planeTransform->setRotationX(planeTransform->getRotation().x + (timer * 10.0f));
		}
		if (Input::IsKeyBeingPressed(S))
		{
			planeTransform->setRotationX(planeTransform->getRotation().x - (timer * 10.0f));
		}
		*/

		if (Input::IsKeyDown(F4))
		{
			mEngine->GetRenderer()->ReloadShaderIncludes();

			for (auto& e : SceneManager::GetInstance()->GetActiveScene()->GetEntities())
			{
				auto& m = e->GetComponents<Material>();
				if (!m.empty())
				{
					for (auto& mat : m)
					{
						mat->ReloadShader();
					}
				}
			}
		}

		if (Input::IsKeyDown(F3))
		{
			Ptr<Shader> wireframeShader = nullptr;
			if (wireframe)
			{
				wireframeShader = CreatePtr<Shader>(
					"shaders/Default.vert", 
					"shaders/PBR.frag");
				wireframe = false;
			}
			else
			{
				wireframeShader = CreatePtr<Shader>(
					"shaders/debug/Wireframe.vert",
					"shaders/debug/PBR_Wireframe.frag",
					"shaders/debug/Wireframe.geom");
				wireframe = true;
			}

			for (auto& e : SceneManager::GetInstance()->GetActiveScene()->GetEntities())
			{
				auto& m = e->GetComponents<Material>();
				if (!m.empty())
				{
					for (auto& mat : m)
					{
						mat->SetShader(wireframeShader);
					}
				}


			}
		}

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

		if(Input::IsKeyDown(KeyCode::D1))
		{
			SceneManager::GetInstance()->SetActiveScene("TestScene");
		}
		if(Input::IsKeyDown(KeyCode::D2))
		{
			SceneManager::GetInstance()->SetActiveScene("SponzaScene");
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

		if(Input::IsKeyDown(C))
		{
			Ptr<Entity> towerEntity = mScene->CreateEntity();
			towerEntity->AddComponent<Mesh>("assets/models/Cerberus.FBX");
			Ptr<Material> towerMat = towerEntity->GetComponent<Material>();
			towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_A.tga", Material::TextureMapType::ALBEDO);
			towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_N.tga", Material::TextureMapType::NORMAL);
			towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_M.tga", Material::TextureMapType::METALLIC);
			towerMat->LoadTexture("assets/textures/Cerberus/Cerberus_R.tga", Material::TextureMapType::ROUGHNESS);
			//
			Ptr<Transform> towerTransform = towerEntity->GetComponent<Transform>();
			//towerTransform->setPosition(0.0f, 5.0f, -30.0f);
			towerTransform->setPosition(mScene->GetCamera()->GetPosition());
			towerTransform->setScale(0.05f, 0.05f, 0.05f);
			towerTransform->setRotation(-90.0f, 0.0f, 0.0f);
		}

		if (Input::IsKeyDown(T))
		{
			// Set up defaults
			time_t unixTime;
			::time(&unixTime);

			struct tm* theTime = localtime(&unixTime);

			int julianDay = theTime->tm_yday;

			float localTime = theTime->tm_hour + theTime->tm_min / 60.0f + theTime->tm_sec / 3600.0f;
			bool dst = (theTime->tm_isdst != 0);


			const Vec2 kLondon(+51.5f, 0.0f);
			const Vec2 kAuckland(-37.0f, 174.8f);
			const Vec2 kPittsburgh(40.5f, -80.22f);
			const Vec2 kOakland(37.8f, -122.2f);
			const Vec2 kSanFrancisco(37.8f, -122.4f);
			const Vec2 kJakarta(-6.21f, 106.85f);
			Vec2 latLong = kLondon;
			float timeZone = rintf(latLong[1] / 15.0f);    // estimate for now

			mScene->SetSunTime(localTime, timeZone, julianDay, latLong[0], latLong[1]);
		}

		static float timer = 0;
		timer += dt;

		if (timer >= 1.0f)
		{
			std::ostringstream ss;
			ss << "Ivy Sandbox v0.0.1 | frame time: ";
			ss << dt * 1000;
			ss << " | FPS: ";
			ss << (1 / dt);
			mEngine->GetWindow()->SetTitle(ss.str());

			timer = 0.0f;
		}

		// Begin new frame
		mEngine->NewFrame();
	}
}
