#include "ivypch.h"
#include "Entity.h"

void Ivy::Entity::UpdateComponents()
{
	for (auto& kv : mComponents)
	{
		for (auto c : kv.second)
		{
			if (c->IsActive())
			{
				c->OnUpdate();
			}
		}
	}
}
