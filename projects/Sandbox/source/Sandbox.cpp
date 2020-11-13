#include <Ivy.h>
#include <chrono>
#include <string>
#include <sstream>
#include <GLFW/glfw3.h>

int main()
{
	using namespace Ivy;

	std::unique_ptr<Engine> engine = std::make_unique<Engine>();
	engine->Initialize(1280, 720, "Ivy Sandbox v0.0.5");

	//Vector<String> skyboxTextures =
	//{
	//	"assets/textures/skybox/right.jpg",
	//	"assets/textures/skybox/left.jpg",
	//	"assets/textures/skybox/top.jpg",
	//	"assets/textures/skybox/bottom.jpg",
	//	"assets/textures/skybox/front.jpg",
	//	"assets/textures/skybox/back.jpg"
	//};
	Vector<String> skyboxTextures =
	{
		"assets/textures/skybox/Default/sky8_RT.jpg",
		"assets/textures/skybox/Default/sky8_LF.jpg",
		"assets/textures/skybox/Default/sky8_UP.jpg",
		"assets/textures/skybox/Default/sky8_DN.jpg",
		"assets/textures/skybox/Default/sky8_BK.jpg",
		"assets/textures/skybox/Default/sky8_FR.jpg",
	};
	Scene::GetScene()->SetSkybox(skyboxTextures);
	//Scene::GetScene()->SetDirectionalLightDirection(Vec3(-2.0f, 2, -0.0f));
	//Scene::GetScene()->AddPointLight(
	//	Vec3(5.0f, 3.0f, 0.0f),
	//	1.0f,
	//	0.09f,
	//	0.032,
	//	Vec3(0.5f, 0.5f, 0.5f),
	//	Vec3(0.8f, 0.8f, 0.8f),
	//	Vec3(1.0f, 1.0f, 1.0f));

	Ptr<Entity> uvSphere = Scene::GetScene()->CreateEntity();
	uvSphere->AddComponent(CreatePtr<Mesh>(uvSphere.get(), "assets/models/ShadowTest.obj"));
	Vector<Ptr<Material>> uvMats = uvSphere->GetComponentsOfType<Material>();
	for(auto& uvMat : uvMats)
	{
		uvMat->LoadTexture("assets/textures/Misc/box.jpg", Material::TextureMapType::DIFFUSE);
	}

	//uvMat->LoadTexture("assets/textures/Misc/brick_normal.jpg", Material::TextureMapType::NORMAL);
	//uvMat->LoadTexture("assets/textures/Cerberus/Cerberus_M.tga", Material::TextureMapType::METALLIC);
	//uvSphere->GetFirstComponentOfType<Transform>()->setScale(0.025f, 0.025f, 0.025f);
	//uvSphere->GetFirstComponentOfType<Transform>()->setScale(20.0f, 20.0f, 20.0f);
	//uvSphere->GetFirstComponentOfType<Transform>()->setRotation(-90.0f, 0.0f, 0.0f);

	Ptr<Entity> tower = Scene::GetScene()->CreateEntity();
	tower->AddComponent(CreatePtr<Mesh>(tower.get(), "assets/models/WatchTower.obj"));
	//Ptr<Material> towerMat = tower->GetFirstComponentOfType<Material>();
	//towerMat->LoadTexture("assets/textures/sponza/sponza_fabric_green_diff.png", Material::TextureMapType::DIFFUSE);
	Ptr<Transform> towerTransform = tower->GetFirstComponentOfType<Transform>();
	towerTransform->setPositionX(40.0f);
	//towerTransform->setScale(10.0f, 10.0f, 10.0f);

	float timer = 0.0f;

	while(!engine->ShouldTerminate())
	{
		if(Input::IsKeyDown(KeyCode::Escape))
		{
			exit(0);
		}

		// Animate the light source
		//Vec3 lightPos = Vec3(0.0f,0.0f, 0.0f);
		//lightPos.x = cos(glm::radians(glfwGetTime() * 360.0f)) * 40.0f;
		//lightPos.y = -50.0f + sin(glm::radians(glfwGetTime() * 360.0f)) * 20.0f;
		//lightPos.z = 25.0f + sin(glm::radians( glfwGetTime() * 360.0f)) * 5.0f;
		//Scene::GetScene()->SetDirectionalLightDirection(lightPos);

		engine->NewFrame();

		
	}

	return 0;
}
