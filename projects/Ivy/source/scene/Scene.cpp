#include "ivypch.h"
#include "Scene.h"

Ivy::Ptr<Ivy::Scene> Ivy::Scene::mInstance = nullptr;

Ivy::Scene::Scene()
{
}

Ivy::Scene::~Scene()
{
}

Ivy::Ptr<Ivy::Entity> Ivy::Scene::CreateEntity()
{
	Ptr<Entity> ent = CreatePtr<Entity>();
	ent->AddComponent(CreatePtr<Transform>());
	ent->AddComponent(CreatePtr<Material>());
	ent->OnCreate();
	ent->mIndex = mEntities.size();
	Debug::CoreLog("Created entity with index {}", ent->mIndex);

	mEntities.push_back(ent);

	return ent;
}

void Ivy::Scene::Update()
{
	for (auto& e : mEntities)
	{
		e->OnUpdate();
		e->UpdateComponents();
	}
}
