#include "ivypch.h"
#include "ParticleSystem.h"

Ivy::ParticleSystem::ParticleSystem()
{
	static const GLfloat vertexBufferData[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};
	mVertexBuffer = CreatePtr<VertexBuffer>();
	mVertexBuffer->SetBufferData(vertexBufferData, sizeof(vertexBufferData));

}
