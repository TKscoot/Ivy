#include "ivypch.h"
#include "Terrain.h"

Ivy::Terrain::Terrain(int width, int height)
	: mWidth(width)
	, mHeight(height)
{

}

void Ivy::Terrain::GenerateMesh()
{

	// Vertices
	mVertices.clear();
	mVertices.resize(mWidth * mHeight);
	int idx = 0;
	mIndices.clear();
/*
	for(int z = 0; z < mHeight; z++)
	{
		for(int x = 0; x < mWidth; x++)
		{
			Vec4 pos = Vec4(x / (float)mWidth, 0.0f, z / (float)mHeight, 1.0f);

			pos.x *= mPointWidth;
			pos.z *= mPointWidth;
			pos.x -= mPointWidth / 2;
			pos.z -= mPointWidth / 2;

			mVertices[z * mWidth + x].position = pos;
			mVertices[z * mWidth + x].texcoord = Vec2((float)x / (float)mWidth, (float)z / (float)mHeight) * mUVScale;
			mVertices[z * mWidth + x].normal = Vec3(0.0f);

			idx++;
		}

	}
*/

	// Size of the terrain in world units
	float terrainWidth = (mWidth - 1) * mPointWidth;
	float terrainHeight = (mHeight - 1) * mPointWidth;

	float halfTerrainWidth = terrainWidth * 0.5f;
	float halfTerrainHeight = terrainHeight * 0.5f;



	for (unsigned int j = 0; j < mHeight; ++j)
	{
		for (unsigned i = 0; i < mWidth; ++i)
		{
			unsigned int index = (j * mWidth) + i;
			float heightValue = 0.0f;// GetHeightValue(&heightMap[index * bytesPerPixel], bytesPerPixel);

			float S = (i / (float)(mWidth - 1));
			float T = (j / (float)(mHeight - 1));

			float X = (S * terrainWidth) - halfTerrainWidth;
			float Y = heightValue * mPointWidth;
			float Z = (T * terrainHeight) - halfTerrainHeight;


			mVertices[index].position = glm::vec4(X, Y, Z, 1.0f);
			mVertices[index].texcoord = Vec2(S, T) * mUVScale;
			mVertices[index].normal   = Vec3(0.0f);
		}
	}




	const uint32_t numTris = (mWidth - 1) * (mHeight - 1) * 2;

	mIndices.clear();
	mIndices.resize(numTris * 3);

	unsigned int index = 0; // Index in the index buffer
	for (unsigned int j = 0; j < (mHeight - 1); ++j)
	{
		for (unsigned int i = 0; i < (mWidth - 1); ++i)
		{
			int vertexIndex = (j * mWidth) + i;
			// Top triangle (T0)
			mIndices[index++] = vertexIndex;                     // V0
			mIndices[index++] = vertexIndex + mWidth + 1;        // V3
			mIndices[index++] = vertexIndex + 1;                 // V1
			// Bottom triangle (T1)
			mIndices[index++] = vertexIndex;                     // V0
			mIndices[index++] = vertexIndex + mWidth;            // V2
			mIndices[index++] = vertexIndex + mWidth + 1;        // V3
		}
	}



	/*
	// Indices
	mIndices.clear();
	for(uint64_t x = 0; x < (mWidth - 1); ++x)
	{
		for(uint64_t y = 0; y < (mHeight - 1); ++y)
		{
			uint64_t i0 = ((y + 0) * mWidth) + (x + 0);
			uint64_t i1 = ((y + 0) * mWidth) + (x + 1);
			uint64_t i2 = ((y + 1) * mWidth) + (x + 0);
			uint64_t i3 = ((y + 1) * mWidth) + (x + 1);

			// Add these indices to a list.
			mIndices.push_back(i2);
			mIndices.push_back(i1);
			mIndices.push_back(i0);
			mIndices.push_back(i2);
			mIndices.push_back(i3);
			mIndices.push_back(i1);
		}
	}
	*/
}

void Ivy::Terrain::SetHeight(int x, int z, float height)
{
	mHeights[z * mWidth + x] = height;
	mVertices[z * mWidth + x].position = Vec4(mVertices[z * mWidth + x].position.x, height, mVertices[z * mWidth + x].position.z, 1.0f);
	
}

float Ivy::Terrain::GetHeight(int x, int z)
{
	return mHeights[z * mWidth + x];
}

float Ivy::Terrain::GetHeightFromWorldPos(Vec3 position)
{
	float height = -FLT_MAX;

	if (mWidth < 2 || mHeight < 2) return height;

	// Width and height of the terrain in world units
	float terrainWidth = (mWidth - 1) * mPointWidth;
	float terrainHeight = (mHeight - 1) * mPointWidth;
	float halfWidth = terrainWidth * 0.5f;
	float halfHeight = terrainHeight * 0.5f;

	// Multiple the position by the inverse of the terrain matrix 
	// to get the position in terrain local space
	glm::vec3 terrainPos = glm::vec3(glm::inverse(mTransform->getComposed()) * glm::vec4(position, 1.0f));
	glm::vec3 invBlockScale(1.0f / mPointWidth, 0.0f, 1.0f / mPointWidth);

	// Calculate an offset and scale to get the vertex indices
	glm::vec3 offset(halfWidth, 0.0f, halfHeight);

	// Get the 4 vertices that make up the triangle we're over
	glm::vec3 vertexIndices = (terrainPos + offset) * invBlockScale;

	int u0 = (int)floorf(vertexIndices.x);
	int u1 = u0 + 1;
	int v0 = (int)floorf(vertexIndices.z);
	int v1 = v0 + 1;

	if (u0 >= 0 && u1 < mWidth && v0 >= 0 && v1 < mHeight)
	{
		glm::vec3 p00 = mVertices[(v0 * mWidth) + u0].position;    // Top-left vertex
		glm::vec3 p10 = mVertices[(v0 * mWidth) + u1].position;    // Top-right vertex
		glm::vec3 p01 = mVertices[(v1 * mWidth) + u0].position;    // Bottom-left vertex
		glm::vec3 p11 = mVertices[(v1 * mWidth) + u1].position;    // Bottom-right vertex

		// Which triangle are we over?
		float percentU = vertexIndices.x - u0;
		float percentV = vertexIndices.z - v0;

		glm::vec3 dU, dV;
		if (percentU > percentV)
		{   // Top triangle
			dU = p10 - p00;
			dV = p11 - p10;
		}
		else
		{   // Bottom triangle
			dU = p11 - p01;
			dV = p01 - p00;
		}

		glm::vec3 heightPos = p00 + (dU * percentU) + (dV * percentV);
		// Convert back to world-space by multiplying by the terrain's world matrix
		heightPos = glm::vec3(mTransform->getComposed() * glm::vec4(heightPos, 1));
		height = heightPos.y;
	}


	return height;
}

void Ivy::Terrain::SetHeights(float* heights)
{
	if(sizeof(heights)/sizeof(*heights) != mWidth * mHeight)
	{
		Debug::CoreError("Heights array is not the size of terrain points! Input Size: {} Actual Size: {}", glm::sqrt(sizeof(heights) / sizeof(*heights)), mWidth * mHeight);
		//return;
	}

	
	for(int z = 0; z < mHeight; z++)
	{
		for(int x = 0; x < mWidth; x++)
		{
			SetHeight(x, z, heights[z * mWidth + x]);
		}
	}
}

void Ivy::Terrain::OnCreate()
{
	mHeights = new float[mWidth * mHeight];
	//RemoveComponentsOfType<Material>();
	mMaterial = AddComponent<TerrainMaterial>();
	mShader = mMaterial->GetShader();

	Ptr<Texture2D> grassTex = CreatePtr<Texture2D>("assets/textures/Scene_Base/Grass002_diffuse.png");
	Ptr<Texture2D> redTex = CreatePtr<Texture2D>("assets/textures/Scene_Base/Stone_rockWall_02_Base_Color.jpg");
	mMaterial->AddTexture(0, grassTex);
	mMaterial->AddTexture(1, redTex);


	GenerateMesh();
	CreateResources();
}

void Ivy::Terrain::OnUpdate(float deltaTime)
{
}

void Ivy::Terrain::OnDraw(Ptr<Camera> camera, Vec2& currentWindowSize)
{
	auto transform = GetComponent<Transform>();
	mShader->Bind();
	mShader->SetUniformMat4("view", camera->GetViewMatrix());
	mShader->SetUniformMat4("projection", camera->GetProjectionMatrix(currentWindowSize));
	mShader->SetUniformMat4("model", transform->getComposed());

	mMaterial->BindTextures();
	mMaterial->UploadUniforms();

	mVertexArray->Bind();
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, (void*)0);
	mVertexArray->Unbind();
}

void Ivy::Terrain::CreateResources()
{
	if (mVertexBuffer) mVertexBuffer->Destroy();
	if (mIndexBuffer) mIndexBuffer->Destroy();
	if (mVertexArray) mVertexArray->Destroy();

	mVertexBuffer = CreatePtr<VertexBuffer>();
	mIndexBuffer = CreatePtr<IndexBuffer>();

	mBufferLayout =
	{
			{ShaderDataType::Float4, "aPosition"},
			{ShaderDataType::Float3, "aNormal"},
			{ShaderDataType::Float3, "aTangent"},
			{ShaderDataType::Float3, "aBitangent"},
			{ShaderDataType::Float2, "aTexCoord"}
	};

	mVertexBuffer = CreatePtr<VertexBuffer>();
	mIndexBuffer = CreatePtr<IndexBuffer>();

	mVertexArray = CreatePtr<VertexArray>(mBufferLayout);
	mVertexArray->SetVertexAndIndexBuffer(mVertexBuffer, mIndexBuffer);

	mVertexArray->Bind();

	mIndexBuffer->Bind();
	mVertexBuffer->SetBufferData(mVertices.data(), mVertices.size() * sizeof(Vertex));

	mIndexBuffer->Bind();
	mIndexBuffer->SetBufferData(mIndices.data(), mIndices.size());

	mVertexBuffer->Unbind();
	mIndexBuffer->Unbind();
	mVertexArray->Unbind();

}

void Ivy::Terrain::CalculateNormals()
{

	for(int i = 0; i < mIndices.size(); i += 3)
	{
		// Get the face normal
		Vec3 vector1 = mVertices[mIndices[(size_t)i + 1]].position - mVertices[mIndices[i]].position;
		Vec3 vector2 = mVertices[mIndices[(size_t)i + 2]].position - mVertices[mIndices[i]].position;
		Vec3 faceNormal = glm::cross(vector1, vector2);
	
		faceNormal = glm::normalize(faceNormal);
	
		// Add the face normal to the 3 vertices normal touching this face
		mVertices[mIndices[i]].normal			  += faceNormal;
		mVertices[mIndices[(size_t)i + 1]].normal += faceNormal;
		mVertices[mIndices[(size_t)i + 2]].normal += faceNormal;
	}
}
