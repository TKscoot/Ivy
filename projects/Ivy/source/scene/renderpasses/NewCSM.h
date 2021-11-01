#pragma once
#include "Types.h"
#include "rendering/Framebuffer.h"
#include "rendering/Shader.h"
#include "scene/Camera.h"
#include "scene/Light.h"
#include "scene/Entity.h"
#include "scene/components/Transform.h"
#include "scene/components/Mesh.h"
#include <imgui.h>

namespace Ivy
{
	class NewCSM
	{
	public:
		NewCSM(Ptr<Camera> camera, DirectionalLight& dirLight)
			: mCamera(camera)
			, mDirLight(dirLight)
		{
			float cameraFarPlane = mCamera->GetFarPlane();

			mShadowCascadeLevels = 
			{ 
				cameraFarPlane / 50.0f, 
				cameraFarPlane / 25.0f, 
				cameraFarPlane / 10.0f, 
				cameraFarPlane / 2.0f 
			};

			mShader = CreatePtr<Shader>("shaders/CSM.vert", "shaders/CSM.frag", "shaders/CSM.geom");

			CreateFramebuffer();
		}

		void RenderToShadowmaps(VecI2 windowSize, Vector<Ptr<Entity>>& entities);

		int GetCascadeCount() { return mShadowCascadeLevels.size(); }

		Vector<float> GetPlaneDistances() { return mShadowCascadeLevels; }

		void BindDepthMap(int slot = 0)
		{
			glBindTextureUnit(slot, mDepthMaps);
		}

		Vector<Mat4> GetLightSpaceMatrices(VecI2 windowSize);

	private:
		void CreateFramebuffer();
		Vector<Vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
		Mat4 GetLightSpaceMatrix(VecI2 windowSize, const float nearPlane, const float farPlane);



		GLuint mFBO		  = 0;
		GLuint mDepthMaps = 0;

		GLuint mMatricesUBO;


		Ptr<Shader> mShader = nullptr; 
		Ptr<Camera> mCamera = nullptr;

		DirectionalLight& mDirLight;

		unsigned int mDepthMapResolution = 4096;

		std::vector<float> mShadowCascadeLevels; // TODO: Evtl Chernos Methode verwenden: dynamischer (ShadowRenderPass.cpp Line 271)
	};
}