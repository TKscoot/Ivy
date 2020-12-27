#include "ivypch.h"
#include "Buffer.h"

// Vertex Buffer

Ivy::VertexBuffer::VertexBuffer()
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}
}

Ivy::VertexBuffer::VertexBuffer(uint32_t size)
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}

	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}

Ivy::VertexBuffer::VertexBuffer(void* vertices, uint32_t size)
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}

	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
}

Ivy::VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &mID);
}

void Ivy::VertexBuffer::CreateBuffer()
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}
}

void Ivy::VertexBuffer::CreateBuffer(uint32_t size)
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}

	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

void Ivy::VertexBuffer::CreateBuffer(void* data, uint32_t size)
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}

	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void Ivy::VertexBuffer::Bind()
{
	if (mID == 0)
	{
		Debug::CoreError("Buffer needs to be created first!");
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, mID);
}

void Ivy::VertexBuffer::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Ivy::VertexBuffer::SetBufferData(const void * data, uint32_t size)
{
	if (mID == 0)
	{
		Debug::CoreError("Buffer needs to be created first!");
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void Ivy::VertexBuffer::SetBufferSubData(uint32_t offset, const void * data, uint32_t size)
{
	if (mID == 0)
	{
		Debug::CoreError("Buffer needs to be created first!");
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}


// Index Buffer

Ivy::IndexBuffer::IndexBuffer()
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}
}

Ivy::IndexBuffer::IndexBuffer(uint32_t count)
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}

	mCount = count;

	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, count, nullptr, GL_DYNAMIC_DRAW);

}

Ivy::IndexBuffer::IndexBuffer(uint32_t * indices, uint32_t count)
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}

	mCount = count;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

Ivy::IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &mID);
}

void Ivy::IndexBuffer::CreateBuffer()
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}
}

void Ivy::IndexBuffer::CreateBuffer(uint32_t size)
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}

	mCount = size;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);
}

void Ivy::IndexBuffer::CreateBuffer(void* data, uint32_t size)
{
	if (mID == 0)
	{
		glCreateBuffers(1, &mID);
	}

	mCount = size;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);
}

void Ivy::IndexBuffer::Bind()
{
	if (mID == 0)
	{
		Debug::CoreError("Buffer needs to be created first!");
		return;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
}

void Ivy::IndexBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Ivy::IndexBuffer::SetBufferData(const void * data, uint32_t size)
{
	if (mID == 0)
	{
		Debug::CoreError("Buffer needs to be created first!");
		return;
	}

	mCount = size;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), data, GL_STATIC_DRAW);
}

void Ivy::IndexBuffer::SetBufferSubData(uint32_t offset, const void * data, uint32_t size)
{
	if (mID == 0)
	{
		Debug::CoreError("Buffer needs to be created first!");
		return;
	}

	mCount += size;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size * sizeof(uint32_t), data);
}
