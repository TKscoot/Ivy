#pragma once
#include "Types.h"
#include "scene/Entity.h"
#include "scene/components/TerrainMaterial.h"
#include "scene/components/Material.h"

#include "Buffer.h"
#include "VertexArray.h"
#include "rendering/Shader.h"

#include <glm/gtc/noise.hpp>

namespace Ivy
{
	class Material;

	class Terrain : public Entity
	{
	public:

		Terrain(int width, int height);


		~Terrain()
		{
			delete[] mHeights;
			mHeights = nullptr;
		}

		void GenerateMesh();

		void SetHeight(int x, int z, float height);
		float GetHeight(int x, int z);

		float GetHeightFromWorldPos(float xPos, float zPos);

		// heights array has to be of size width * height of terrain
		void SetHeights(float* heights);

		
		VecI2 GetTerrainDimensions() { return VecI2(mWidth, mHeight); }

		void OnCreate() override;
		void OnUpdate(float deltaTime) override;
		void OnDraw(Ptr<Camera> camera, Vec2& currentWindowSize) override;

		void CreateResources();
		void CalculateNormals();

		Ptr<TerrainMaterial> GetMaterial() { return mMaterial; }
	private:

		int				  mWidth  = 512;
		int				  mHeight = 512;
		float			  mUVScale = 16.0f;

		const float		  mPointWidth = 250.0f;

		float baseFreq = 4.0f;
		float persistence = 0.5f;

		float*			  mHeights;

		Ptr<TerrainMaterial> mMaterial = nullptr;

		// Rendering
		Vector<Vertex>    mVertices;
		Vector<uint32_t>  mIndices;
		Vector<std::array<Vec3, 3>>	  mFaces;

		Ptr<Shader>		  mShader;

		Ptr<VertexBuffer> mVertexBuffer;
		Ptr<IndexBuffer>  mIndexBuffer;

		Ptr<VertexArray>  mVertexArray;

		BufferLayout      mBufferLayout = {};

	};
}
