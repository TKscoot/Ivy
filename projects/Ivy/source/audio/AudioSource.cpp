#include "ivypch.h"
#include "AudioSource.h"

void Ivy::AudioSource::ShouldLoop(bool loop)
{
	alSourcei(mSource, AL_LOOPING, (ALint)loop);
	AlCheckError();
}

bool Ivy::AudioSource::IsPlaying()
{
	ALint state;
	alGetSourcei(mSource, AL_SOURCE_STATE, &state);
	AlCheckError();
	return (state == AL_PLAYING);
}

void Ivy::AudioSource::SetGain(float val)
{
	float newval = val;
	if(newval < 0)
		newval = 0;
	alSourcef(mSource, AL_GAIN, val);
	AlCheckError();
}
