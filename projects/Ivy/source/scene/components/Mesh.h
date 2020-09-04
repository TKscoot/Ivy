#pragma once
#include "Types.h"

#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"
#include <glad/glad.h>

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
        void CreateResources();
        void Draw();

        Vector<Vertex>   mVertices;
        Vector<uint32_t> mIndices;

        Ptr<VertexBuffer> mVertexBuffer;
        Ptr<IndexBuffer>  mIndexBuffer;

        Ptr<VertexArray>  mVertexArray;

        BufferLayout      mBufferLayout;
    };
} // namespace Ivy
