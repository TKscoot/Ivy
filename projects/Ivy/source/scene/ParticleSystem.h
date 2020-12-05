#pragma once
#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"

namespace Ivy
{
	class ParticleSystem
	{
	public:
		ParticleSystem();

	private:


		Ptr<VertexBuffer> mVertexBuffer = nullptr;
		Ptr<VertexArray>  mVertexArray  = nullptr;

	};

}