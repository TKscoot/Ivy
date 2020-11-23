#pragma once
#include <Ivy.h>

using namespace Ivy;

class CameraTracker : public Entity
{
public:
	CameraTracker(Ptr<Camera> camera);

	void OnCreate() final;
	void OnUpdate(float deltaTime) final;
	void OnDestroy() final;

	void AddTrackingPoint(Vec3 point);
	void AddTrackingPoint(Vec3 point, int index);

	void ShouldPlay(bool play) { mShouldPlay = play; }

private:
	Ptr<Camera>  mCamera		 = nullptr;
	Vector<Vec3> mTrackingPoints = {};
	float		 mTimer			 = 0.0f;
	float		 mDuration		 = 3.0f;
	int			 mTrackingIndex  = 0;

	bool		 mShouldPlay		 = true;
};