#include <Ivy.h>
#include <chrono>
#include <string>
#include <sstream>

int main()
{
	using namespace Ivy;

	float timer = 0;

	std::unique_ptr<Engine> engine = std::make_unique<Engine>();
	engine->Initialize(800, 600, "Ivy Sandbox v0.0.1");
	
	Ptr<Entity> uvSphere = Scene::GetScene()->CreateEntity();
	uvSphere->AddComponent(CreatePtr<Mesh>("assets/models/sponza.obj"));
	Ptr<Material> uvMat = uvSphere->GetFirstComponentOfType<Material>();
	uvSphere->GetFirstComponentOfType<Transform>()->setScale(0.2f, 0.2f, 0.2f);
	//Ptr<Texture2D> texture = Texture2D::Create("assets/textures/box.jpg");
	//
	//uvMat->SetTexture(texture, Material::DIFFUSE);


	Ptr<Entity> bunny = Scene::GetScene()->CreateEntity();
	bunny->AddComponent(CreatePtr<Mesh>("assets/models/teapot.obj"));
	Ptr<Transform> bunnyTransform = bunny->GetFirstComponentOfType<Transform>();
	bunnyTransform->setPositionX(30.0f);

	while (!engine->ShouldTerminate())
	{
		if (Input::IsKeyDown(KeyCode::Escape))
		{
			exit(0);
		}

		timer += engine->GetDeltaTime();
		//bunnyTransform->setRotationY(timer * 10);


		engine->NewFrame();

		// TODO: remove and fix shader initialization in material class
		uvMat->SetAmbientColor(Vec3(0.5f, 0.5f, 0.5f));
		uvMat->SetDiffuseColor(Vec3(1.0f, 1.0f, 1.0f));
		uvMat->SetSpecularColor(Vec3(0.5f, 0.5f, 0.5f));
		uvMat->SetShininess(32.0f);
	}

	return 0;
}