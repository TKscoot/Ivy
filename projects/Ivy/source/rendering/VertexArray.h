#pragma once
#include "Buffer.h"

namespace Ivy
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void Bind();
		void Unbind();

		void SetVertexBuffer(Ptr<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer( Ptr<IndexBuffer>& indexBuffer);

		const Ptr<VertexBuffer> GetVertexBuffer() { return mVertexBuffer; }
		const Ptr<IndexBuffer>  GetIndexBuffer() { return mIndexBuffer; }

	private:
		GLuint mID;
		Ptr<VertexBuffer> mVertexBuffer;
		Ptr<IndexBuffer> mIndexBuffer;
	};
}

