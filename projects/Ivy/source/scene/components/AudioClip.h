#pragma once
#include "audio/AudioContext.h"
#include "audio/AudioStreamSource.h"
#include "Component.h"
#include "scene/Entity.h"
#include "taskflow.hpp"
#include "core/AsyncWorker.h"


namespace Ivy
{
	class AudioClip : public Component
	{
	public:
		enum AudioSourceType
		{
			EFFECT,
			MUSIC
		};

		AudioClip(Entity* ent, String filename, bool playOnStart = false);

		void AddSoundClip(String filename);
		void Play();
		void Pause();
		void Resume();
		void Stop();

		void ShouldLoop(bool loop);

		void PlayOnStart(bool shouldPlay) { mPlayOnStart = shouldPlay; }

		void OnStart() override;
		void OnUpdate(float deltatime) override;


	private:
		bool mPlayOnStart = false;

		Ptr<AudioStreamSource> mAudioStream = nullptr;
	};
}