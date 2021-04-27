#pragma once
#include "SkyModel.h"

namespace Ivy
{
	class HosekWilkieSkyModel : public SkyModel
	{
	public:
		HosekWilkieSkyModel();
		~HosekWilkieSkyModel();

		void Initialize() override;
		void Update() override;
		void SetRenderUniforms(Ptr<Shader> program) override;

	private:
		// Methods
		Vec3   HosekWilkie(
			float cos_theta, 
			float gamma, 
			float cos_gamma, 
			glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, glm::vec3 E, glm::vec3 F, glm::vec3 G, glm::vec3 H, glm::vec3 I);
		double Evaluate(const double * dataset, size_t stride, float turbidity, float albedo, float sunTheta);
		double EvaluateSpline(const double * spline, size_t stride, double value);
		
		
		glm::vec3 A, B, C, D, E, F, G, H, I;
		glm::vec3 Z;
	};
}
