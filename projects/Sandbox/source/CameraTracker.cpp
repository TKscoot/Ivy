#include "CameraTracker.h"

CameraTracker::CameraTracker(Ptr<Camera> camera)
	: mCamera(camera)
{
}

void CameraTracker::OnCreate()
{
	//mCamera->SetRotation(180.0f);
	mCamera->SetPosition(Vec3(0.0f, 2.0f, -10.0f));
}

void CameraTracker::OnUpdate(float deltaTime)
{
	if(!mShouldPlay) return;

	if(mTrackingIndex >= mTrackingPoints.size())
	{
		mTrackingIndex = 0;
	}

	if(mTimer < 1.0f)
	{
		mTimer += deltaTime;
		Vec3 position = mCamera->GetPosition();

		position = Lerp(position, mTrackingPoints[mTrackingIndex], mTimer * 0.05);
		mCamera->SetPosition(position);
		Debug::Log("x:{} y:{} z{}", position.x, position.y, position.z);
	}
	else
	{
		mTrackingIndex++;
		mTimer = 0.0f;
	}
}

void CameraTracker::OnDestroy()
{
}

void CameraTracker::AddTrackingPoint(Vec3 point)
{
	mTrackingPoints.push_back(point);
}

void CameraTracker::AddTrackingPoint(Vec3 point, int index)
{
	auto itPos = mTrackingPoints.begin() + index;
	mTrackingPoints.insert(itPos, point);
}
