#include "ivypch.h"
#include "Mesh.h"
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>

Ivy::Mesh::Mesh()
{
	mEnt = Scene::GetScene()->GetEntityWithIndex(GetEntityIndex());
    CreateResources();
}

Ivy::Mesh::Mesh(String filepath)
{
	mEnt = Scene::GetScene()->GetEntityWithIndex(GetEntityIndex());

    CreateResources();
    Load(filepath);
}

Ivy::Mesh::Mesh(Vector<Vertex> vertices, Vector<uint32_t> indices)
{
	mEnt = Scene::GetScene()->GetEntityWithIndex(GetEntityIndex());

}

void Ivy::Mesh::Load(String filepath)
{
	uint32_t flags =
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate;


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

	Vector<Ptr<Material>> materials = mEnt->GetComponentsOfType<Material>();

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		Submesh& submesh = mSubmeshes[i].second;

		submesh.vertices.resize(mesh->mNumVertices);
		submesh.indices.resize(mesh->mNumFaces * 3);
		submesh.index = i;
		submesh.materialIndex = mesh->mMaterialIndex;

		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			submesh.vertices[j].position = mesh->HasPositions()
				? Vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f)
				: Vec4(0.0f);
			submesh.vertices[j].normal = mesh->HasNormals()
				? Vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z)
				: Vec3(0.0f);
			submesh.vertices[j].tangent = mesh->HasTangentsAndBitangents()
				? Vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z)
				: Vec3(0.0f);
			submesh.vertices[j].texcoord = mesh->HasTextureCoords(0)
				? Vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y)
				: Vec2(0.0f);
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
			{ShaderDataType::Float2, "aTexCoord", submesh.vertexOffset}
		};
		mSubmeshes[i].first = CreatePtr<VertexArray>(layout);
		mSubmeshes[i].first->SetVertexAndIndexBuffer(mVertexBuffer, mIndexBuffer);

		mSubmeshes[i].first->Bind();
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
			materials.push_back(mEnt->AddComponent(CreatePtr<Material>()));

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
		assimpMaterial->GetTexture(aiTextureType_NORMALS,   0, &norm);
		assimpMaterial->GetTexture(aiTextureType_SHININESS, 0, &rough);
		assimpMaterial->GetTexture(aiTextureType_AMBIENT,   0, &metal);

		std::stringstream ss;
		ss << "assets/textures/";
		ss << diff.C_Str();
		
		if (diff.length > 0)
		{
			// TODO: Check if texture has already been loaded
			materials[mesh->mMaterialIndex]->LoadTexture(ss.str(), Material::TextureMapType::DIFFUSE);
		}

		//ss.clear();
		//ss << norm.C_Str();
		//mat->LoadTexture(ss.str(), Material::TextureMapType::NORMAL);
		//
		//ss.clear();
		//ss << rough.C_Str();
		//mat->LoadTexture(ss.str(), Material::TextureMapType::ROUGHNESS);
		//
		//ss.clear();
		//ss << metal.C_Str();
		//mat->LoadTexture(ss.str(), Material::TextureMapType::METALLIC);
		
	}

}

void Ivy::Mesh::Draw()
{
    //mVertexArray->Bind();
    //glDrawElements(GL_TRIANGLES, mIndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
	
	auto& materials = mEnt->GetComponentsOfType<Material>();

	for (int i = 0; i < mSubmeshes.size(); i++)
	{
		if (materials.size() >= mSubmeshes[i].second.materialIndex)
		{
			for (auto& kv : materials[mSubmeshes[i].second.materialIndex]->GetTextures())
			{
				kv.second->Bind(static_cast<uint32_t>(kv.first));
			}
		}

		mSubmeshes[i].first->Bind();
		glDrawElements(GL_TRIANGLES, mSubmeshes[i].second.indices.size(), GL_UNSIGNED_INT, (void*)mSubmeshes[i].second.indexOffset);
	}

}

void Ivy::Mesh::CreateResources()
{
    mVertexBuffer = CreatePtr<VertexBuffer>();
    mIndexBuffer  = CreatePtr<IndexBuffer>();
}
