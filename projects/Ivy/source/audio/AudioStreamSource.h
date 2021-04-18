#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <sndfile.h>
#include "OpenALHelpers.h"
#include "audio/FModContext.h"
#include "AudioContext.h"
#include "AudioSource.h"
#include "Types.h"

namespace Ivy
{
	class AudioStreamSource : public AudioSource
	{
	public:
		AudioStreamSource(String filename);

		void Play() override;
		void Pause() override;
		void Stop() override;
		void Resume() override;

		void Update() override
		{
		};

		void AddSoundClip(String filename) override;

	private:
		FMOD::Sound* mSound = nullptr;
		FMOD::Channel* mChannel = nullptr;

	};
}