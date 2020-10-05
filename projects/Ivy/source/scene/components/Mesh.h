#pragma once


#include "Types.h"
#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"
#include "Component.h"
#include "scene/Entity.h"
#include "scene/components/Material.h"
#include "scene/Scene.h"


namespace Ivy
{
    class Renderer;

    class Mesh : public Component
    {
    public:
        friend class Renderer;

		struct Submesh
		{
			uint32_t         index		  = 0;
			uint32_t         vertexOffset = 0;
			uint32_t         indexOffset  = 0;

			Vector<Vertex>   vertices;
			Vector<uint32_t> indices;

			uint32_t		 materialIndex = 0;

			Ptr<VertexArray> vertexArray;
		};

		struct less_than_key
		{
			inline bool operator() (const Submesh& struct1, const Submesh& struct2)
			{
				return (struct1.materialIndex < struct2.materialIndex);
			}
		};


        Mesh();
        Mesh(String filepath);
        Mesh(Vector<Vertex> vertices, Vector<uint32_t> indices);

        //void Load(String filepath);
		void Load(String filepath);

    private:
        void CreateResources();
        void Draw();

        //Vector<Vertex>   mVertices;
        //Vector<uint32_t> mIndices;

		Vector<Submesh> mSubmeshes;
		UnorderedMap<String, Ivy::Ptr<Texture2D>> mLoadedTextures;

        Ptr<VertexBuffer> mVertexBuffer;
        Ptr<IndexBuffer>  mIndexBuffer;

        Ptr<VertexArray>  mVertexArray;

        BufferLayout      mBufferLayout;

		Ptr<Entity> mEnt;
    };
} // namespace Ivy
