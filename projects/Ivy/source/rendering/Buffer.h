#pragma once
#include "Types.h"
#include "environment/Log.h"

namespace Ivy
{
	class Buffer
	{
	public:
		virtual ~Buffer() = default;

		virtual void CreateBuffer() = 0;
		virtual void CreateBuffer(uint32_t size) = 0;
		virtual void CreateBuffer(float* data, uint32_t size) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetBufferData(const void* data, uint32_t size) = 0;

		GLuint GetHandle() { return mID; }

	protected:
		GLuint mID = 0;
	};

	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer();
		VertexBuffer(uint32_t size);
		VertexBuffer(float* vertices, uint32_t size);

		virtual ~VertexBuffer();


		virtual void CreateBuffer() override;
		virtual void CreateBuffer(uint32_t size) override;
		virtual void CreateBuffer(float* data, uint32_t size) override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void SetBufferData(const void* data, uint32_t size) override;
	};

	class IndexBuffer : public Buffer
	{
	public:
		IndexBuffer();
		IndexBuffer(uint32_t count);
		IndexBuffer(uint32_t* indices, uint32_t count);

		virtual ~IndexBuffer();

		virtual void CreateBuffer() override;
		virtual void CreateBuffer(uint32_t size) override;
		virtual void CreateBuffer(float* data, uint32_t size) override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void SetBufferData(const void* data, uint32_t size) override;

		uint32_t GetCount() const { return mCount; }
	
	private:
		uint32_t mCount = 0;
	};

}