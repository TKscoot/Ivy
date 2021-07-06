#include "ivypch.h"
#include "AudioListener.h"

Ivy::AudioListener::AudioListener(Ptr<Entity> entity, Ptr<Camera> camera) : Component::Component(entity)
{
	mCamera = camera;

	mContext = AudioContext::GetInstance();

	UpdateValues();
}

void Ivy::AudioListener::OnStart()
{
	mTransform = mCamera->GetTransform();
}

void Ivy::AudioListener::OnUpdate(float deltaTime)
{
	UpdateValues();


}

void Ivy::AudioListener::UpdateValues()
{
	mPosition   = mCamera->GetTransform()->getPosition();
	Vec3 lookAt = mCamera->GetLookAt();

	mContext->GetEngine()->setListenerPosition(mContext->Vec3ToVec3df(mPosition), mContext->Vec3ToVec3df(lookAt));

}
