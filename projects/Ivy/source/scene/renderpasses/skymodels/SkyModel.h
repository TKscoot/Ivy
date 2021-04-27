#pragma once
#include "Types.h"
#include "rendering/Shader.h"

namespace Ivy
{
	class SkyModel
	{
	public:
		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void SetRenderUniforms(Ptr<Shader>) = 0;

		inline Vec3 Direction() { return mDirection; }
		inline void SetDirection(Vec3 dir) { mDirection = dir; }

		inline float Turbidity() { return mTurbidity; }
		inline void SetTurbidity(float t) { mTurbidity = t; }

	protected:
		Vec3  mDirection;
		float mNormalizedSunY = 1.15f;
		float mAlbedo		  = 0.1f;
		float mTurbidity	  = 4.0f;
	};
}