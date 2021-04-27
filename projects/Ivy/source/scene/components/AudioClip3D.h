#pragma once
#include "AudioClip.h"

namespace Ivy
{
	class AudioClip3D : public AudioClip
	{
	public:
		AudioClip3D(Ptr<Transform> transform, String filename, bool playOnStart = false);

		virtual void AddSoundClip(String filename);


		void OnSoundStopped(ISound* sound, E_STOP_EVENT_CAUSE reason, void* userData) override;

	private:


		Ptr<Transform> mTransform = nullptr;
	};
}