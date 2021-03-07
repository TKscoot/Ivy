#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include "OpenALHelpers.h"
#include "Types.h"

namespace Ivy
{
	class AudioSource
	{
	public:
		virtual void AddSoundClip(String filename) = 0;
		virtual void Play() = 0;
		virtual void Pause() = 0;
		virtual void Resume() = 0;
		virtual void Stop() = 0;

		virtual void Update() = 0;

		void ShouldLoop(bool loop);
		bool IsPlaying();
		void SetGain(float val);

	protected:
		ALuint mSource = 0;
		ALuint mBuffer = 0;

		SNDFILE* mSndFile;
		SF_INFO  mSfinfo;
		short*   mMembuf;
		ALenum   mFormat;

		ALboolean loop = AL_FALSE;
	};
}
