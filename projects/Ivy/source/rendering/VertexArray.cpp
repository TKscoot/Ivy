#include "ivypch.h"
#include "VertexArray.h"

Ivy::VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &mID);
}

Ivy::VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &mID);
}

void Ivy::VertexArray::Bind()
{
	glBindVertexArray(mID);
}

void Ivy::VertexArray::Unbind()
{
	glBindVertexArray(0);
}

void Ivy::VertexArray::SetVertexBuffer(Ptr<VertexBuffer>& vertexBuffer)
{
	if (!vertexBuffer)
	{
		Debug::CoreError("VertexBuffer is Null!");
		return;
	}

	mVertexBuffer = vertexBuffer;
	glBindVertexArray(mID);
	mVertexBuffer->Bind();
	
	// TODO: vertexBuffer->GetLayout() (layout class??)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // vec3 position
	glEnableVertexAttribArray(0);

	mVertexBuffer->Unbind();
}

void Ivy::VertexArray::SetIndexBuffer(Ptr<IndexBuffer>& indexBuffer)
{
	if (!indexBuffer)
	{
		Debug::CoreError("IndexBuffer is Null!");
		return;
	}

	mIndexBuffer = indexBuffer;
	glBindVertexArray(mID);
	mIndexBuffer->Bind();
}
