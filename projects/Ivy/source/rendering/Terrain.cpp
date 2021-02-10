#include "ivypch.h"
#include "Terrain.h"

void Ivy::Terrain::GenerateMesh()
{
	// Vertices
	for(int z = 0; z < mHeight; z++)
	{
		for(int x = 0; x < mWidth; x++)
		{
			Vertex& vert = mVertices[z*mWidth +x];
			vert.position = Vec4(x, 0.0f, z, 1.0f);
			vert.texcoord = Vec2((float)x / mWidth, (float)z / mHeight) * mUVScale;
		}
		
	}

	// Indices
	mIndices.clear();
	for(int z = 0; z < mHeight; z++)
	{
		for(int x = 0; x < mWidth; x++)
		{
			uint64_t i0 = ((x + 0) * mWidth) + (z + 0);
			uint64_t i1 = ((x + 0) * mWidth) + (z + 1);
			uint64_t i2 = ((x + 1) * mWidth) + (z + 0);
			uint64_t i3 = ((x + 1) * mWidth) + (z + 1);
			
			// Add these indices to a list.
			mIndices.push_back(i2);
			mIndices.push_back(i1);
			mIndices.push_back(i0);
			mIndices.push_back(i2);
			mIndices.push_back(i3);
			mIndices.push_back(i1);
		}
	}
}
