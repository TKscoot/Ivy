#include "ivypch.h"
#include "Mesh.h"
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>

Ivy::Mesh::Mesh(Entity* ent) : Ivy::Component::Component(ent)
{
	//mEnt = Scene::GetScene()->GetEntityWithIndex(GetEntityIndex());
    CreateResources();
}

Ivy::Mesh::Mesh(Entity* ent, String filepath) : Ivy::Component::Component(ent)
{
	//uint32_t entidx = GetEntityIndex();
	//Debug::CoreLog("entidx: {}", entidx);
	//mEnt = Scene::GetScene()->GetEntityWithIndex(entidx); //TODO: GetEntityIndex not working properly

    CreateResources();
    Load(filepath);
}

Ivy::Mesh::Mesh(Entity* ent, Vector<Vertex> vertices, Vector<uint32_t> indices) : Ivy::Component::Component(ent)
{
	//mEnt = Scene::GetScene()->GetEntityWithIndex(GetEntityIndex());
}

void Ivy::Mesh::Load(String filepath)
{
	uint32_t flags =
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_RemoveRedundantMaterials;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filepath.c_str(), flags);

	if (!scene)
	{
		Debug::CoreError("Couldn't load mesh file!");
		return;
	}

	if (!scene->HasMeshes())
	{
		Debug::CoreError("Mesh is empty/has no vertices");
		return;
	}

	mSubmeshes.resize(scene->mNumMeshes);

	uint32_t totalVertexCount = 0;
	uint32_t totalIndexCount = 0;

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		totalVertexCount += mesh->mNumVertices;
		totalIndexCount += mesh->mNumFaces * 3;

	}

	mVertexBuffer->Bind();
	mVertexBuffer->SetBufferData(nullptr, totalVertexCount * sizeof(Vertex));
	mIndexBuffer->Bind();
	mIndexBuffer->SetBufferData(nullptr, totalIndexCount);

	uint32_t currentVertexOffset = 0;
	uint32_t currentIndexOffset = 0;

	Vector<Ptr<Material>> materials = mEntity->GetComponentsOfType<Material>();

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		Submesh& submesh = mSubmeshes[i];

		submesh.vertices.resize(mesh->mNumVertices);
		submesh.indices.resize(mesh->mNumFaces * 3);
		submesh.index = i;
		submesh.materialIndex = mesh->mMaterialIndex;

		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			if (mesh->HasPositions())
			{
				submesh.vertices[j].position = Vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f);
			}
			if (mesh->HasNormals())
			{
				submesh.vertices[j].normal    = Vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			}																	   
			if (mesh->HasTangentsAndBitangents())								   
			{																	   
				submesh.vertices[j].tangent   = Vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z);
																				   
				submesh.vertices[j].bitangent = Vec3(mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z); 
			}
			if (mesh->HasTextureCoords(0))
			{
				submesh.vertices[j].texcoord  = Vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			}
		}

		for (uint32_t j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace* face = mesh->mFaces + j;

			for (uint32_t k = 0; k < face->mNumIndices; k++)
			{
				submesh.indices[((3 * j) + k)] = (*(face->mIndices + k));
			}
		}

		submesh.vertexOffset = currentVertexOffset * sizeof(Vertex);  // BUGGY: Eventuell = currentVertex(Index)Offset + 1;
		submesh.indexOffset = currentIndexOffset * sizeof(uint32_t);

		BufferLayout layout =
		{
			{ShaderDataType::Float4, "aPosition", submesh.vertexOffset},
			{ShaderDataType::Float3, "aNormal"  , submesh.vertexOffset},
			{ShaderDataType::Float3, "aTangent" , submesh.vertexOffset},
			{ShaderDataType::Float3, "aBitangent", submesh.vertexOffset},
			{ShaderDataType::Float2, "aTexCoord", submesh.vertexOffset}
		};
	
		submesh.vertexArray = CreatePtr<VertexArray>(layout);
		submesh.vertexArray->SetVertexAndIndexBuffer(mVertexBuffer, mIndexBuffer);
		submesh.vertexArray->Bind();

		mVertexBuffer->Bind();
		mVertexBuffer->SetBufferSubData(submesh.vertexOffset, submesh.vertices.data(), submesh.vertices.size() * sizeof(Vertex));
		mIndexBuffer->Bind();
		mIndexBuffer->SetBufferSubData(submesh.indexOffset, submesh.indices.data(), submesh.indices.size());

		currentVertexOffset += mesh->mNumVertices;
		currentIndexOffset += mesh->mNumFaces * 3;
	}
	//material assignment
	if (scene->HasMaterials())
	{
		for (int i = 1; i < scene->mNumMaterials; i++)
		{
			materials.push_back(mEntity->AddComponent(CreatePtr<Material>()));

		}
	}

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		// Get the material
		aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
		aiString diff;
		aiString norm;
		aiString rough;
		aiString metal;
		assimpMaterial->GetTexture(aiTextureType_DIFFUSE,   0, &diff);
		assimpMaterial->GetTexture(aiTextureType_HEIGHT,    0, &norm);
		assimpMaterial->GetTexture(aiTextureType_SHININESS, 0, &rough);
		assimpMaterial->GetTexture(aiTextureType_AMBIENT,   0, &metal);

		{
			std::stringstream ss;
			ss << "assets/textures/";
			ss << diff.C_Str();

			if (diff.length > 0)
			{
				materials[mesh->mMaterialIndex]->LoadTexture(ss.str(), Material::TextureMapType::DIFFUSE);
			}
		}
		{
			std::stringstream ss;
			ss << "assets/textures/";
			ss << norm.C_Str();
			if (norm.length > 0)
			{
				materials[mesh->mMaterialIndex]->LoadTexture(ss.str(), Material::TextureMapType::NORMAL);
			}
		}
		//
		//ss.clear();
		//ss << rough.C_Str();
		//mat->LoadTexture(ss.str(), Material::TextureMapType::ROUGHNESS);
		//
		//ss.clear();
		//ss << metal.C_Str();
		//mat->LoadTexture(ss.str(), Material::TextureMapType::METALLIC);
		
	}
	
	// sorting submeshes for better texture binding performance
	std::sort(mSubmeshes.begin(), mSubmeshes.end(), less_than_key());
}

void Ivy::Mesh::Draw()
{
    //mVertexArray->Bind();
    //glDrawElements(GL_TRIANGLES, mIndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
	
	auto& materials = mEntity->GetComponentsOfType<Material>();

	static uint32_t lastMatIndex = 0;
	int bindCount = 0;
	for (int i = 0; i < mSubmeshes.size(); i++)
	{

		// TODO: VERY PERFORMANCE HUNGRY: FIX
		if (materials.size() >= mSubmeshes[i].materialIndex)
		{
			for (auto& kv : materials[mSubmeshes[i].materialIndex]->GetTextures())
			{
				if (mSubmeshes[i].materialIndex != lastMatIndex || mSubmeshes[i].materialIndex == 0)
				{
					kv.second->Bind(static_cast<uint32_t>(kv.first));
					bindCount++;
				}
			}

			lastMatIndex = mSubmeshes[i].materialIndex;
		}
		// ENDTODO

		mSubmeshes[i].vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, mSubmeshes[i].indices.size(), GL_UNSIGNED_INT, (void*)mSubmeshes[i].indexOffset);
	}
}

void Ivy::Mesh::CreateResources()
{
    mVertexBuffer = CreatePtr<VertexBuffer>();
    mIndexBuffer  = CreatePtr<IndexBuffer>();
}
