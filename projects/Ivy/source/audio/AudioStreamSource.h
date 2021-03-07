#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <sndfile.h>
#include "OpenALHelpers.h"
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
			UpdateBufferStream();
		};

		void AddSoundClip(String filename) override;

	private:
		void UpdateBufferStream();


		static const int BUFFER_SAMPLES = 8192;
		static const int NUM_BUFFERS = 4;
		ALuint mBuffers[NUM_BUFFERS];
	};
}