#pragma once
#include "audio/AudioContext.h"
#include "Component.h"
#include "scene/Entity.h"

namespace Ivy
{
	class AudioListener : public Component
	{
	public:
		AudioListener(Ptr<Camera> camera);

		void OnStart() override;
		void OnUpdate(float deltaTime) override;

	private:
		void UpdateValues();
		Ptr<AudioContext> mContext = nullptr;

		Ptr<Camera> mCamera = nullptr;

		Ptr<Transform> mTransform = nullptr;
		Vec3 mPosition = Vec3(0.0f);
		Vec3 mVelocity = Vec3(0.0f);
	
	};
}

