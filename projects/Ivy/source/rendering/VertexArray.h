#pragma once
#include "Buffer.h"

namespace Ivy
{
	class VertexArray
	{
	public:
		VertexArray(BufferLayout& layout);
		~VertexArray();

		void Bind();
		void Unbind();

		void SetVertexBuffer(Ptr<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer( Ptr<IndexBuffer>& indexBuffer);
		void SetVertexAndIndexBuffer(Ptr<VertexBuffer>& vertexBuffer, Ptr<IndexBuffer>& indexBuffer);

		const Ptr<VertexBuffer> GetVertexBuffer() { return mVertexBuffer; }
		const Ptr<IndexBuffer>  GetIndexBuffer() { return mIndexBuffer; }


		void SetLayout(BufferLayout& layout)
		{
			mBufferLayout = layout;
		}

		inline const BufferLayout GetLayout()
		{
			return mBufferLayout;
		}

		void EnableVertexAttributes();
	private:

		GLuint mID;
		BufferLayout mBufferLayout;
		Ptr<VertexBuffer> mVertexBuffer;
		Ptr<IndexBuffer> mIndexBuffer;
	};
}

