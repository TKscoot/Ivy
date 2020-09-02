#pragma once
#include "Types.h"

#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"


namespace Ivy
{
	class Renderer;
	
	class Mesh
	{
	public:
		friend class Renderer;

		Mesh();
		Mesh(String filepath);
		Mesh(Vector<Vertex> vertices, Vector<uint32_t> indices);

		void Load(String filepath);


	private:
		Vector<Vertex>	 mVertices;
		Vector<uint32_t> mIndices;

		Ptr<Buffer>		 mVertexBuffer;
		Ptr<Buffer>		 mIndexBuffer;

		Ptr<VertexArray> mVertexArray;

		BufferLayout     mBufferLayout;
	};
}