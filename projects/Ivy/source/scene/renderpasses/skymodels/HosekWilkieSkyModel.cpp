#include "ivypch.h"
#include "HosekWilkieSkyModel.h"
#include "HosekDataRGB.h"

namespace Ivy
{
	// -----------------------------------------------------------------------------------------------------------------------------------


	double HosekWilkieSkyModel::EvaluateSpline(const double* spline, size_t stride, double value)
	{
		return
			1 * pow(1 - value, 5) *                 spline[0 * stride] +
			5 * pow(1 - value, 4) * pow(value, 1) * spline[1 * stride] +
			10 * pow(1 - value, 3) * pow(value, 2) * spline[2 * stride] +
			10 * pow(1 - value, 2) * pow(value, 3) * spline[3 * stride] +
			5 * pow(1 - value, 1) * pow(value, 4) * spline[4 * stride] +
			1 * pow(value, 5) * spline[5 * stride];
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	double HosekWilkieSkyModel::Evaluate(const double * dataset, size_t stride, float turbidity, float albedo, float sunTheta)
	{
		// splines are functions of elevation^1/3
		double elevationK = pow(std::max<float>(0.f, 1.f - sunTheta / (glm::pi<float>() / 2.f)), 1.f / 3.0f);

		// table has values for turbidity 1..10
		int turbidity0 = glm::clamp(static_cast<int>(turbidity), 1, 10);
		int turbidity1 = std::min(turbidity0 + 1, 10);
		float turbidityK = glm::clamp(turbidity - turbidity0, 0.f, 1.f);

		const double * datasetA0 = dataset;
		const double * datasetA1 = dataset + stride * 6 * 10;

		double a0t0 = EvaluateSpline(datasetA0 + stride * 6 * (turbidity0 - 1), stride, elevationK);
		double a1t0 = EvaluateSpline(datasetA1 + stride * 6 * (turbidity0 - 1), stride, elevationK);
		double a0t1 = EvaluateSpline(datasetA0 + stride * 6 * (turbidity1 - 1), stride, elevationK);
		double a1t1 = EvaluateSpline(datasetA1 + stride * 6 * (turbidity1 - 1), stride, elevationK);

		return a0t0 * (1 - albedo) * (1 - turbidityK) + a1t0 * albedo * (1 - turbidityK) + a0t1 * (1 - albedo) * turbidityK + a1t1 * albedo * turbidityK;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	glm::vec3 HosekWilkieSkyModel::HosekWilkie(float cos_theta, float gamma, float cos_gamma, glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, glm::vec3 E, glm::vec3 F, glm::vec3 G, glm::vec3 H, glm::vec3 I)
	{
		glm::vec3 chi = (1.f + cos_gamma * cos_gamma) / pow(1.f + H * H - 2.f * cos_gamma * H, glm::vec3(1.5f));
		return (1.f + A * exp(B / (cos_theta + 0.01f))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * (float)sqrt(std::max(0.f, cos_theta)));
	}

	// -----------------------------------------------------------------------------------------------------------------------------------


	HosekWilkieSkyModel::HosekWilkieSkyModel()
	{
		Initialize();
	}

	HosekWilkieSkyModel::~HosekWilkieSkyModel()
	{
	}

	void HosekWilkieSkyModel::Initialize()
	{
		mDirection = glm::vec3(0.0f, sin(35), cos(35));
		//Serialize();
		Deserialize();
	}

	void HosekWilkieSkyModel::Update()
	{
		const float sunTheta = std::acos(glm::clamp(mDirection.y, 0.f, 1.f));

		for (int i = 0; i < 3; ++i)
		{
			A[i] = Evaluate(datasetsRGB[i].data() + 0, 9, mTurbidity, mAlbedo, sunTheta);
			B[i] = Evaluate(datasetsRGB[i].data() + 1, 9, mTurbidity, mAlbedo, sunTheta);
			C[i] = Evaluate(datasetsRGB[i].data() + 2, 9, mTurbidity, mAlbedo, sunTheta);
			D[i] = Evaluate(datasetsRGB[i].data() + 3, 9, mTurbidity, mAlbedo, sunTheta);
			E[i] = Evaluate(datasetsRGB[i].data() + 4, 9, mTurbidity, mAlbedo, sunTheta);
			F[i] = Evaluate(datasetsRGB[i].data() + 5, 9, mTurbidity, mAlbedo, sunTheta);
			G[i] = Evaluate(datasetsRGB[i].data() + 6, 9, mTurbidity, mAlbedo, sunTheta);

			// Swapped in the dataset
			H[i] = Evaluate(datasetsRGB[i].data() + 8, 9, mTurbidity, mAlbedo, sunTheta);
			I[i] = Evaluate(datasetsRGB[i].data() + 7, 9, mTurbidity, mAlbedo, sunTheta);

			Z[i] = Evaluate(datasetsRGBRad[i].data(), 1, mTurbidity, mAlbedo, sunTheta);
		}

		if (mNormalizedSunY)
		{
			glm::vec3 S = HosekWilkie(std::cos(sunTheta), 0, 1.f, A, B, C, D, E, F, G, H, I) * Z;
			Z /= glm::dot(S, glm::vec3(0.2126, 0.7152, 0.0722));
			Z *= mNormalizedSunY;
		}

	}

	void HosekWilkieSkyModel::SetRenderUniforms(Ptr<Shader> program)
	{
		program->SetUniformFloat3("Direction", mDirection);
		program->SetUniformFloat3("A", A);
		program->SetUniformFloat3("B", B);
		program->SetUniformFloat3("C", C);
		program->SetUniformFloat3("D", D);
		program->SetUniformFloat3("E", E);
		program->SetUniformFloat3("F", F);
		program->SetUniformFloat3("G", G);
		program->SetUniformFloat3("H", H);
		program->SetUniformFloat3("I", I);
		program->SetUniformFloat3("Z", Z);
	}

}

