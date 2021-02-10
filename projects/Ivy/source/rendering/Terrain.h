#pragma once
#include "Types.h"

namespace Ivy
{
	class Terrain
	{
	public:

		Terrain(int width, int height) 
		: mWidth(width)
		, mHeight(height)
		{
			mHeights = new float[width*height];

			mVertices.resize(width*height);
		}

		~Terrain()
		{
			delete[] mHeights;
			mHeights = nullptr;
		}

		void GenerateMesh();


	private:
		int mWidth  = 512;
		int mHeight = 512;
		float mUVScale = 64.0f;

		Vector<Vertex>   mVertices;
		Vector<uint32_t> mIndices;
		float*			 mHeights;

	};
}
