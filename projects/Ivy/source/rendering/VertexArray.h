#pragma once
#include "Buffer.h"

namespace Ivy
{
	class VertexArray
	{
	public:
		/*!
		 * Creates VertexArray object to store vertex and index buffers
		 * 
		 * \param layout The data layout of the GPU buffer
		 */
		VertexArray(BufferLayout& layout);
		/*!
		 * Destructor
		 * Cleans up all resources of the VertexArray
		 * 
		 */
		~VertexArray();

		/*!
		 * Tells the renderer to bind and use this vertex array
		 * 
		 */
		void Bind();
		/*!
		 * Tells the renderer to no longer use this vertex array
		 * 
		 */
		void Unbind();

		/*!
		 * Sets the vertex buffer with it's data
		 * 
		 * \param vertexBuffer VertexBuffer object to use
		 */
		void SetVertexBuffer(Ptr<VertexBuffer>& vertexBuffer);
		
		/*!
		 * Sets the index buffer with it's data
		 *
		 * \param indexBuffer IndexBuffer object to use
		 */
		void SetIndexBuffer( Ptr<IndexBuffer>& indexBuffer);
		/*!
		 * Sets both vertex and index buffers with it's data
		 * 
		 * \param vertexBuffer VertexBuffer object to use
		 * \param indexBuffer IndexBuffer object to use
		 */
		void SetVertexAndIndexBuffer(Ptr<VertexBuffer>& vertexBuffer, Ptr<IndexBuffer>& indexBuffer);

		/*!
		 * 
		 * \return Gets the vertex buffer object
		 */
		const Ptr<VertexBuffer> GetVertexBuffer() { return mVertexBuffer; }
		
		/*!
		 *
		 * \return Gets the index buffer object
		 */
		const Ptr<IndexBuffer>  GetIndexBuffer() { return mIndexBuffer; }

		/*!
		 * Tells the renderer which buffer layout to use
		 * 
		 * \param layout Buffer layout object to use
		 */
		void SetLayout(BufferLayout& layout)
		{
			mBufferLayout = layout;
		}

		/*!
		 * 
		 * \return Returns the currently used buffer layout of this vertex array
		 */
		inline const BufferLayout GetLayout()
		{
			return mBufferLayout;
		}

		/*!
		 * Enables the vertex attributes with the specified set buffer layout
		 * 
		 */
		void EnableVertexAttributes();
	private:

		GLuint mID;
		BufferLayout mBufferLayout;
		Ptr<VertexBuffer> mVertexBuffer;
		Ptr<IndexBuffer> mIndexBuffer;
	};
}

