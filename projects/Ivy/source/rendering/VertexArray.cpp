#include "ivypch.h"
#include "VertexArray.h"

Ivy::VertexArray::VertexArray(BufferLayout& layout)
{
	mBufferLayout = layout;
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
	Bind();
	mVertexBuffer->Bind();

	EnableVertexAttributes();

	Unbind();
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

	Unbind();
}

void Ivy::VertexArray::SetVertexAndIndexBuffer(Ptr<VertexBuffer>& vertexBuffer, Ptr<IndexBuffer>& indexBuffer)
{
	mVertexBuffer = vertexBuffer;
	mIndexBuffer  = indexBuffer;
	Bind();

	mVertexBuffer->Bind();
	mIndexBuffer->Bind();

	EnableVertexAttributes();

	Unbind();
}

void Ivy::VertexArray::EnableVertexAttributes()
{
	const auto& elements = mBufferLayout.GetElements();
	uint32_t offset = 0;
	for (uint32_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
 		uint32_t offset = element.Offset + element.BufferOffset;
		Debug::CoreLog("offset of {} is {}", i, offset);

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,
			element.GetComponentCount(),
			ShaderDataTypeToOpenGLBaseType(element.Type),
			element.Normalized ? GL_TRUE : GL_FALSE,
			mBufferLayout.GetStride(),
			(void*)(offset));
	}
}
