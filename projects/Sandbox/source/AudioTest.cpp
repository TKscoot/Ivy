#include "AudioTest.h"

AudioTest::AudioTest()
{
	for(auto& p : std::filesystem::recursive_directory_iterator("assets\\sounds"))
	{
		mSoundFiles.push_back(p.path().string());
	}
}

void AudioTest::OnCreate()
{
	mAudioClip = CreatePtr<AudioClip>(this, "assets/sounds/ophelia.mp3", true);
	AddComponent<AudioClip>(mAudioClip);
}

void AudioTest::OnUpdate(float deltaTime)
{
	ImGui::Begin("Audio");


	static String current = "";
	if(ImGui::BeginCombo("##combo", current.c_str()))
	{
		for(auto& p : mSoundFiles)
		{
			bool is_selected = (current == p); // You can store your selection however you want, outside or inside your objects
			if(ImGui::Selectable(p.c_str(), is_selected))
			{
				current = p.c_str();
				mAudioClip->AddSoundClip(p);
			}
			if(is_selected)
				ImGui::SetItemDefaultFocus();
		}
		
		ImGui::EndCombo();
	}



	bool isFinished = false;// mAudioClip->IsFinished();
	ImGui::Text("Finished: %d", isFinished);

	if(ImGui::Button("Play"))
	{
		mAudioClip->Play();
	}

	if(ImGui::Button("Pause"))
	{
		mAudioClip->Pause();
	}

	if(ImGui::Button("Resume"))
	{
		mAudioClip->Resume();
	}

	if(ImGui::Button("Stop"))
	{
		mAudioClip->Stop();
	}

	float volume		= mAudioClip->GetVolume();
	float pan			= mAudioClip->GetPan();
	float speed			= mAudioClip->GetPlaybackSpeed();
	int duration		= mAudioClip->GetDuration();
	int currentPosition = mAudioClip->GetPlaybackPosition();
	bool loop			= mAudioClip->IsLooping();

	if(ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f))
	{
		mAudioClip->SetVolume(volume);
	}

	if(ImGui::SliderFloat("Pan", &pan, 0.0f, 1.0f))
	{
		mAudioClip->SetPan(pan);
	}

	if(ImGui::SliderFloat("Speed", &speed, 0.0f, 10.0f))
	{
		mAudioClip->SetPlaybackSpeed(speed);
	}

	if(ImGui::SliderInt("Position", &currentPosition, 0, duration))
	{
		mAudioClip->SetPlaybackPosition(currentPosition);
	}

	if(ImGui::Checkbox("Loop", &loop))
	{
		mAudioClip->ShouldLoop(loop);
	}


	/*
	GetVolume();

	SetVolume(float volume);

	GetPan();

	SetPan(float pan);

	IsFinished();

	GetDuration();

	GetPlaybackPosition();

	SetPlaybackPosition(int position);

	GetPlaybackSpeed();
	SetPlaybackSpeed(float speed);
	*/
	ImGui::End();
}

void AudioTest::OnDestroy()
{
}
