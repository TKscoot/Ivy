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
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // vec3 position
	//glEnableVertexAttribArray(0);

	const auto& layout = vertexBuffer->GetLayout();
	const auto& elements = layout.GetElements();
	uint32_t offset = 0;
	for (uint32_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, 
			element.GetComponentCount(), 
			ShaderDataTypeToOpenGLBaseType(element.Type), 
			element.Normalized ? GL_TRUE : GL_FALSE,
			layout.GetStride(), 
			(const void*)element.Offset);
	}

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
