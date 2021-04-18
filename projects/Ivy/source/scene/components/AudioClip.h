#pragma once
#include "audio/AudioContext.h"
#include "audio/AudioStreamSource.h"
#include "Component.h"
#include "scene/Entity.h"
#include "taskflow.hpp"
#include "core/AsyncWorker.h"


namespace Ivy
{
	class AudioClip : public Component, public ISoundStopEventReceiver
	{
	public:
		enum AudioSourceType
		{
			EFFECT,
			MUSIC
		};

		AudioClip(Entity* ent, String filename, bool playOnStart = false);

		virtual void AddSoundClip(String filename);
		virtual void Play();
		virtual void Pause();
		virtual void Resume();
		virtual void Stop();

		void OnSoundStopped(ISound* sound, E_STOP_EVENT_CAUSE reason, void* userData) override;




		float GetVolume();

		void  SetVolume(float volume);

		float GetPan();

		void  SetPan(float pan);

		bool  IsFinished();

		int	  GetDuration();

		int   GetPlaybackPosition();

		void  SetPlaybackPosition(int position);

		float GetPlaybackSpeed();
		void  SetPlaybackSpeed(float speed);




		void ShouldLoop(bool loop);
		bool IsLooping();

		void PlayOnStart(bool shouldPlay) { mPlayOnStart = shouldPlay; }

		void OnStart() override;
		void OnUpdate(float deltatime) override;


	private:
		String mFilePath = "";

		bool mPlayOnStart = false;

		ISound* mSound = nullptr;
		Ptr<AudioContext> mContext = nullptr;

		ik_u32 mPlayPositionAfterPause = 0;
	};
}