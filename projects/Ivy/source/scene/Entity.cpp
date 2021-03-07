#include "ivypch.h"
#include "Entity.h"

void Ivy::Entity::UpdateComponents(float deltaTime)
{
	for (auto& kv : mComponents)
	{
		for (auto c : kv.second)
		{
			if (c->IsActive())
			{
				c->OnUpdate(deltaTime);
			}
		}
	}
}

void Ivy::Entity::StartComponents()
{
	for(auto& kv : mComponents)
	{
		for(auto c : kv.second)
		{
			if(c->IsActive())
			{
				c->OnStart();
			}
		}
	}
}
