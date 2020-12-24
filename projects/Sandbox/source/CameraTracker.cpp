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
	ImGui::Begin("CameraTracker");
	ImGui::Checkbox("Play", &mShouldPlay);
	ImGui::SliderFloat("Speed", &mDuration, 0.01f, 10.0f);
	if(ImGui::Button("Add current camera position"))
	{
		AddTrackingPoint(mCamera->GetPosition());
	}
	ImGui::BeginChild("child_1", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
	for(int i = 0; i < mTrackingPoints.size(); i++)
	{
		bool isCurrent = false;
		if(i == mTrackingIndex) isCurrent = true;

		std::stringstream ss;
		ss << "x: " << mTrackingPoints[i].x;
		ss << " y: " << mTrackingPoints[i].y;
		ss << " z: " << mTrackingPoints[i].z;
		ImGui::Selectable(ss.str().c_str(), &isCurrent); 
	}
	ImGui::EndChild();
	ImGui::End();

	if(!mShouldPlay || mTrackingPoints.size() == 0) return;

	if(mTrackingIndex >= mTrackingPoints.size())
	{
		mTrackingIndex = 0;
	}

	if(mTimer < 1.0f)
	{
		mTimer += deltaTime;
		Vec3 position = mCamera->GetPosition();
		position = Lerp(position, mTrackingPoints[mTrackingIndex], mTimer);
		mCamera->SetPosition(position);
		Debug::Log("{}", mTimer);
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
