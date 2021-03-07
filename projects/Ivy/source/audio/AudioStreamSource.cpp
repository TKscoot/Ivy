#include "ivypch.h"
#include "AudioStreamSource.h"

Ivy::AudioStreamSource::AudioStreamSource(String filename)
{
	AddSoundClip(filename);
}

void Ivy::AudioStreamSource::Play()
{
	ALsizei i;

	/* Rewind the source position and clear the buffer queue */
	alSourceRewind(mSource);
	alSourcei(mSource, AL_BUFFER, 0);

	/* Fill the buffer queue */
	for(i = 0; i < NUM_BUFFERS; i++)
	{
		/* Get some data to give it to the buffer */
		sf_count_t slen = sf_readf_short(mSndFile, mMembuf, BUFFER_SAMPLES);
		if(slen < 1) break;

		slen *= mSfinfo.channels * (sf_count_t)sizeof(short);
		alBufferData(mBuffers[i], mFormat, mMembuf, (ALsizei)slen, mSfinfo.samplerate);
	}
	if(alGetError() != AL_NO_ERROR)
	{
		Debug::CoreError("Error buffering for playback");
	}

	/* Now queue and start playback! */
	alSourceQueueBuffers(mSource, i, mBuffers);
	alSourcePlay(mSource);
	if(alGetError() != AL_NO_ERROR)
	{
		Debug::CoreError("Error starting playback");
	}
}

void Ivy::AudioStreamSource::Pause()
{
	alSourcePause(mSource);
	AlCheckError();
}

void Ivy::AudioStreamSource::Stop()
{
	alSourceStop(mSource);
	AlCheckError();
}

void Ivy::AudioStreamSource::Resume()
{
	alSourcePlay(mSource);
	AlCheckError();
}

void Ivy::AudioStreamSource::UpdateBufferStream()
{
	ALint processed, state;
	/* Get relevant source info */
	alGetSourcei(mSource, AL_SOURCE_STATE, &state);
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
	AlCheckError();

	/* Unqueue and handle each processed buffer */
	while(processed > 0)
	{
		ALuint bufid;
		sf_count_t slen;

		alSourceUnqueueBuffers(mSource, 1, &bufid);
		processed--;

		/* Read the next chunk of data, refill the buffer, and queue it
		 * back on the source */
		slen = sf_readf_short(mSndFile, mMembuf, BUFFER_SAMPLES);
		if(slen > 0)
		{
			slen *= mSfinfo.channels * (sf_count_t)sizeof(short);
			alBufferData(bufid, mFormat, mMembuf, (ALsizei)slen,
				mSfinfo.samplerate);
			alSourceQueueBuffers(mSource, 1, &bufid);
		}
		if(alGetError() != AL_NO_ERROR)
		{
			Debug::CoreError("error buffering music data");
		}
	}

	/* Make sure the source hasn't underrun */
	if(state != AL_PLAYING && state != AL_PAUSED)
	{
		ALint queued;

		/* If no buffers are queued, playback is finished */
		alGetSourcei(mSource, AL_BUFFERS_QUEUED, &queued);
		AlCheckError();
		if(queued == 0)
			return;

		alSourcePlay(mSource);
		AlCheckError();
	}
}

void Ivy::AudioStreamSource::AddSoundClip(String filename)
{
	alGenSources(1, &mSource);
	alGenBuffers(NUM_BUFFERS, mBuffers);

	std::size_t frame_size;

	mSndFile = sf_open(filename.c_str(), SFM_READ, &mSfinfo);
	if(!mSndFile)
	{
		Debug::CoreError("could not open provided music file -- check path");
	}

	/* Get the sound format, and figure out the OpenAL format */
	if(mSfinfo.channels == 1)
		mFormat = AL_FORMAT_MONO16;
	else if(mSfinfo.channels == 2)
		mFormat = AL_FORMAT_STEREO16;
	else if(mSfinfo.channels == 3)
	{
		if(sf_command(mSndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			mFormat = AL_FORMAT_BFORMAT2D_16;
	}
	else if(mSfinfo.channels == 4)
	{
		if(sf_command(mSndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			mFormat = AL_FORMAT_BFORMAT3D_16;
	}
	if(!mFormat)
	{
		sf_close(mSndFile);
		mSndFile = NULL;
		Debug::CoreError("Unsupported channel count from file");
	}

	frame_size = ((size_t)BUFFER_SAMPLES * (size_t)mSfinfo.channels) * sizeof(short);
	mMembuf = static_cast<short*>(malloc(frame_size));
}
