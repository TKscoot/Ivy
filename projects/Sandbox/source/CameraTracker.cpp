#include "CameraTracker.h"

CameraTracker::CameraTracker(Ptr<Camera> camera)
	: mCamera(camera)
{

}

void CameraTracker::OnCreate()
{
	mCamera->SetPosition(Vec3(0.0f, 2.0f, -10.0f));
}

void CameraTracker::OnUpdate(float deltaTime)
{
	ImGui::Begin("CameraTracker");
	static Vec3 lookAt = Vec3(0.0f);
	static float radius = 10.0f;
	static float rotationSpeed = 10.0f;
	Vec3 camPos = mCamera->GetPosition();
	ImGui::Text("Camera Position: x: %.00f, y: %.00f, z: %.00f", camPos.x, camPos.y, camPos.z);

	ImGui::Spacing();

	if(ImGui::Combo("Camera mode", &mCurrentCameraMode, mCameraModes.data(), mCameraModes.size()))
	{
		if(mCurrentCameraMode == 1)
		{
			mCamera->Reset();
		}
	}
	if(mCurrentCameraMode == 1)
	{
		ImGui::SliderFloat("Rotation speed", &rotationSpeed, 1.0f, 100.0f);
		ImGui::SliderFloat("Circling Radius", &radius, 0.001f, 100.0f);
	}
	else if(mCurrentCameraMode == 2)
	{
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
	}
	ImGui::End();
	
	if(mCurrentCameraMode == 1)
	{
		mShouldPlay = false;

		mTimer += deltaTime * rotationSpeed;

		static Vec3 circlePoint = Vec3(0.0f);
		circlePoint.x = radius * glm::sin(glm::radians(mTimer));
		circlePoint.y = mCamera->GetPosition().y;
		circlePoint.z = radius * glm::cos(glm::radians(mTimer));

		mCamera->LookAt(Vec3(0.0f, circlePoint.y, 0.0f));
		mCamera->SetPosition(circlePoint);
	}
	else if(mCurrentCameraMode == 2)
	{
		if(!mShouldPlay || mTrackingPoints.size() == 0) return;

		if(mTrackingIndex >= mTrackingPoints.size())
		{
			mTrackingIndex = 0;
		}

		if(mTimer < 1.0f)
		{
			mTimer += deltaTime * mDuration;
			Vec3 position = mCamera->GetPosition();
			position = Lerp(position, mTrackingPoints[mTrackingIndex], mTimer);
			mCamera->SetPosition(position);
		}
		else
		{
			mTrackingIndex++;
			mTimer = 0.0f;
		}
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
