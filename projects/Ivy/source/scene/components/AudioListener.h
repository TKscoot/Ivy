#pragma once
#include "audio/AudioContext.h"
#include "Component.h"
#include "scene/Entity.h"


namespace Ivy
{
	class AudioListener : public Component
	{
	public:
		AudioListener(Entity* ent);

		void OnUpdate(float deltaTime) override;

	private:
		void UpdateValues();

		Ptr<Transform> mTransform = nullptr;
		std::array<ALfloat, 6> mListenerOrientation = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
		Vec3 mPosition = Vec3(0.0f);
		Vec3 mVelocity = Vec3(0.0f);
	
	};
}

