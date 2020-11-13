#pragma once
#include <Types.h>

namespace Ivy
{
	struct DirectionalLight
	{
		Vec3 direction = Vec3(0.0f);

		Vec3 ambient   = Vec3(0.0f);
		Vec3 diffuse   = Vec3(0.0f);
		Vec3 specular  = Vec3(0.0f);
	};

	struct PointLight
	{
		Vec3 position	= Vec3(0.0f);

		float constant	= 0.0f;
		float linear	= 0.0f;
		float quadratic = 0.0f;

		Vec3 ambient	= Vec3(0.0f);
		Vec3 diffuse	= Vec3(0.0f);
		Vec3 specular	= Vec3(0.0f);
	};

	struct SpotLight
	{
		Vec3 position	  = Vec3(0.0f);
		Vec3 direction	  = Vec3(0.0f);
		float cutOff	  = 0.0f;
		float outerCutOff = 0.0f;

		float constant	  = 0.0f;
		float linear	  = 0.0f;
		float quadratic	  = 0.0f;

		Vec3 ambient	  = Vec3(0.0f);
		Vec3 diffuse	  = Vec3(0.0f);
		Vec3 specular	  = Vec3(0.0f);
	};
}