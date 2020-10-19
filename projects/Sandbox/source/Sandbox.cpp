#include <Ivy.h>
#include <chrono>
#include <string>
#include <sstream>

int main()
{
	using namespace Ivy;

	std::unique_ptr<Engine> engine = std::make_unique<Engine>();
	engine->Initialize(800, 600, "Ivy Sandbox v0.0.1");
	
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

	Ptr<Entity> uvSphere = Scene::GetScene()->CreateEntity();
	uvSphere->AddComponent(CreatePtr<Mesh>(uvSphere.get(), "assets/models/sponza.obj"));
	Ptr<Material> uvMat = uvSphere->GetFirstComponentOfType<Material>();
	//uvMat->LoadTexture("assets/textures/box.jpg", Material::TextureMapType::DIFFUSE);
	//uvMat->LoadTexture("assets/textures/brick_normal.jpg", Material::TextureMapType::NORMAL);
	uvSphere->GetFirstComponentOfType<Transform>()->setScale(0.085f, 0.085f, 0.085f);

	Ptr<Entity> bunny = Scene::GetScene()->CreateEntity();
	bunny->AddComponent(CreatePtr<Mesh>(bunny.get(), "assets/models/wooden watch tower2.obj"));
	//auto m = bunny->GetFirstComponentOfType<Material>();
	//m->LoadTexture("assets/textures/box.jpg", Material::DIFFUSE);
	Ptr<Transform> bunnyTransform = bunny->GetFirstComponentOfType<Transform>();
	bunnyTransform->setPositionX(30.0f);

	while (!engine->ShouldTerminate())
	{
		if (Input::IsKeyDown(KeyCode::Escape))
		{
			exit(0);
		}

		engine->NewFrame();

		// TODO: remove and fix shader initialization in material class
		//uvMat->SetAmbientColor( Vec3(0.5f, 0.5f, 0.5f));
		//uvMat->SetDiffuseColor( Vec3(1.0f, 1.0f, 1.0f));
		//uvMat->SetSpecularColor(Vec3(0.5f, 0.5f, 0.5f));
		//uvMat->SetShininess(32.0f);
	}

	return 0;
}
