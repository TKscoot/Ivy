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
	mVertices.resize(mWidth * mHeight);
	int idx = 0;
	mIndices.clear();

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
	
			Vec3 p0 = mVertices[i0].position;
			Vec3 p1 = mVertices[i1].position;
			Vec3 p2 = mVertices[i2].position;

			Vec3 v1 = p1 - p0;
			Vec3 v2 = p2 - p0;

			Vec3 dot = glm::cross(v1, v2);

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

void Ivy::Terrain::SetHeight(int x, int z, float height)
{
	mHeights[z * mWidth + x] = height;
	mVertices[z * mWidth + x].position = Vec4(mVertices[z * mWidth + x].position.x, height, mVertices[z * mWidth + x].position.z, 1.0f);
	
}

float Ivy::Terrain::GetHeight(int x, int z)
{
	return mHeights[z * mWidth + x];
}

float Ivy::Terrain::GetHeightFromWorldPos(float xPos, float zPos)
{
	// Transform from terrain local space to "cell" space.
	auto transform = GetFirstComponentOfType<Transform>();

	Vec3 worldPos = Vec3(xPos, 0.0f, zPos) - transform->getPosition();



	Vec3 twd;
	twd.x = (mWidth - 1) * mPointWidth;
	twd.y = 0.0f;
	twd.z = (mHeight - 1) * mPointWidth;
	
	Vec3 pos;
	pos.x = ((worldPos.x - transform->getPosition().x) / twd.x);// * mWidth;
	pos.z = ((worldPos.z - transform->getPosition().z) / twd.z);// * mHeight;

	Vec3 terPos;
	terPos.x = mWidth * pos.x;
	terPos.z = mHeight * pos.z;

	terPos.x = (xPos + 0.5f * ((mWidth - 1) * mPointWidth)) / mPointWidth;
	terPos.z = (zPos - 0.5f * ((mHeight - 1) * mPointWidth)) / -mPointWidth;

	int x = glm::floor(terPos.x);
	int z = glm::floor(terPos.z);


	Debug::CoreLog("x: {} y: {}", terPos.x, terPos.z);

	
	return mHeights[x * mWidth + z];
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
	auto transform = GetFirstComponentOfType<Transform>();
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

void Ivy::Terrain::CalculateNormals(int x, int z)
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
	
	// Normalize vertex normal
	//for(int i = 0; i < mVertices.size(); i++)
	//{
	//	mVertices[i].normal = glm::normalize(mVertices[i].normal);
	//}
}
