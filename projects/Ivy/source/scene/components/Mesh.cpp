#include "ivypch.h"
#include "Mesh.h"
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>

Ivy::UnorderedMap<Ivy::String, Ivy::Ptr<Ivy::Mesh>> Ivy::Mesh::mLoadedMeshes = {};


Ivy::Mesh::Mesh(Entity* ent) : Ivy::Component::Component(ent)
{
	//mEnt = Scene::GetScene()->GetEntityWithIndex(GetEntityIndex());
    CreateResources();
}

Ivy::Mesh::Mesh(Entity* ent, String filepath, bool useMtlIfProvided) : Ivy::Component::Component(ent)
{
	//uint32_t entidx = GetEntityIndex();
	//Debug::CoreLog("entidx: {}", entidx);
	//mEnt = Scene::GetScene()->GetEntityWithIndex(entidx); //TODO: GetEntityIndex not working properly

    CreateResources();
    Load(filepath, useMtlIfProvided);
}

Ivy::Mesh::Mesh(Entity* ent, Vector<Vertex> vertices, Vector<uint32_t> indices) : Ivy::Component::Component(ent)
{
	//mEnt = Scene::GetScene()->GetEntityWithIndex(GetEntityIndex());
}

void Ivy::Mesh::Load(String filepath, bool useMtlIfProvided)
{
	mMeshName = filepath;

	RemoveUntilCharacterInString(    mMeshName, '/' );
	RemoveUntilCharacterInString(    mMeshName, '\\');
	RemoveFromCharacterInStringToEnd(mMeshName, '.' );

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

	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float minZ = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float maxY = std::numeric_limits<float>::min();
	float maxZ = std::numeric_limits<float>::min();
	Vec3 minVec = Vec3(0.0f);
	Vec3 maxVec = Vec3(0.0f);

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		Submesh& submesh = mSubmeshes[i];

		submesh.vertices.resize(mesh->mNumVertices);
		//submesh.indices.resize(mesh->mNumFaces * 3);
		submesh.index = i;
		submesh.materialIndex = mesh->mMaterialIndex;

		Vector<Vertex> vertices;
		Vector<unsigned int> indices;

		// walk through each of the mesh's vertices
		for (unsigned int j = 0; j < mesh->mNumVertices; j++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[j].x;
			vector.y = mesh->mVertices[j].y;
			vector.z = mesh->mVertices[j].z;
			vertex.position = Vec4(vector, 1.0f);
			// normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[j].x;
				vector.y = mesh->mNormals[j].y;
				vector.z = mesh->mNormals[j].z;
				vertex.normal = vector;
			}
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][j].x;
				vec.y = mesh->mTextureCoords[0][j].y;
				vertex.texcoord = vec;
				// tangent
				vector.x = mesh->mTangents[j].x;
				vector.y = mesh->mTangents[j].y;
				vector.z = mesh->mTangents[j].z;
				vertex.tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[j].x;
				vector.y = mesh->mBitangents[j].y;
				vector.z = mesh->mBitangents[j].z;
				vertex.bitangent = vector;
			}
			else
				vertex.texcoord = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace face = mesh->mFaces[j];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int k = 0; k < face.mNumIndices; k++)
				indices.push_back(face.mIndices[k]);
		}

		submesh.vertices = vertices;
		submesh.indices = indices;


		// Bounding Box Calculation

		/*
		for (auto& vert : vertices)
		{
			Vec3 pos = vert.position;
			if (pos.x < minX) minX = pos.x;
			if (pos.y < minY) minY = pos.y;
			if (pos.z < minZ) minZ = pos.z;

			if (pos.x > maxX) maxX = pos.x;
			if (pos.y > maxY) maxY = pos.y;
			if (pos.z > maxZ) maxZ = pos.z;
		}

		minVec = Vec3(minX, minY, minZ);
		maxVec = Vec3(maxX, maxY, maxZ);
		*/


		submesh.vertexOffset = currentVertexOffset * sizeof(Vertex);  // BUGGY: Eventuell = currentVertex(Index)Offset + 1;
		submesh.indexOffset  = currentIndexOffset * sizeof(uint32_t);
			
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

		submesh.vertexArray->GetVertexBuffer()->Bind();
		submesh.vertexArray->GetVertexBuffer()->SetBufferSubData(submesh.vertexOffset, submesh.vertices.data(), submesh.vertices.size() * sizeof(Vertex));
		submesh.vertexArray->GetIndexBuffer()->Bind();
		submesh.vertexArray->GetIndexBuffer()->SetBufferSubData(submesh.indexOffset, submesh.indices.data(), submesh.indices.size());

		mVertexBuffer->Unbind();
		mIndexBuffer->Unbind();
		submesh.vertexArray->Unbind();

		currentVertexOffset += submesh.vertices.size();
		currentIndexOffset  += submesh.indices.size();
	}

	//material assignment
	if(useMtlIfProvided)
	{

		if(scene->HasMaterials())
		{
			for(int i = 1; i < scene->mNumMaterials; i++)
			{
				materials.push_back(mEntity->AddComponent(CreatePtr<Material>()));

			}
		}

		for(uint32_t i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];

			// Get the material
			aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
			aiString diff;
			aiString norm;
			aiString rough;
			aiString metal;
			assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diff);
			assimpMaterial->GetTexture(aiTextureType_HEIGHT, 0, &norm);
			assimpMaterial->GetTexture(aiTextureType_SHININESS, 0, &rough);
			assimpMaterial->GetTexture(aiTextureType_AMBIENT, 0, &metal);

			{
				std::stringstream ss;
				ss << "assets/textures/";
				ss << mMeshName;
				ss << "/";
				ss << diff.C_Str();

				if(diff.length > 0)
				{
					materials[mesh->mMaterialIndex]->LoadTexture(ss.str(), Material::TextureMapType::DIFFUSE);
				}
			}
			{
				std::stringstream ss;
				ss << "assets/textures/";
				ss << mMeshName;
				ss << "/";
				ss << norm.C_Str();
				if(norm.length > 0)
				{
					materials[mesh->mMaterialIndex]->LoadTexture(ss.str(), Material::TextureMapType::NORMAL);
				}
			}
			{
				std::stringstream ss;
				ss << "assets/textures/";
				ss << mMeshName;
				ss << "/";
				ss << rough.C_Str();
				if(rough.length > 0)
				{
					materials[mesh->mMaterialIndex]->LoadTexture(ss.str(), Material::TextureMapType::ROUGHNESS);
				}
			}
			{
				std::stringstream ss;
				ss << "assets/textures/";
				ss << mMeshName;
				ss << "/";
				ss << metal.C_Str();
				if(metal.length > 0)
				{
					materials[mesh->mMaterialIndex]->LoadTexture(ss.str(), Material::TextureMapType::METALLIC);
				}
			}

		}
	}
	// sorting submeshes for better texture binding performance
	std::sort(mSubmeshes.begin(), mSubmeshes.end(), less_than_key());
}

void Ivy::Mesh::Draw(bool bindTextures)
{
	if (!IsActive()) return;

	auto& materials = mEntity->GetComponentsOfType<Material>();

	static uint32_t lastMatIndex = 0;
	for (int i = 0; i < mSubmeshes.size(); i++)
	{

		// TODO: VERY PERFORMANCE HUNGRY WHEN LOTS OF TEXTUREBINDS: FIX
		if (bindTextures && materials.size() >= mSubmeshes[i].materialIndex)
		{
			for (auto& kv : materials[mSubmeshes[i].materialIndex]->GetTextures())
			{
				//if (mSubmeshes[i].materialIndex != lastMatIndex || mSubmeshes[i].materialIndex == 0)
				//{
				//	kv.second->Bind(static_cast<uint32_t>(kv.first));
				//}
				textures[static_cast<uint32_t>(kv.first)] = kv.second->GetRendererID();
			}

			glBindTextures(0, textures.size(), textures.data());
			
			materials[mSubmeshes[i].materialIndex]->GetShader()->Bind();
			materials[mSubmeshes[i].materialIndex]->UpdateShaderTextureBools();
			materials[mSubmeshes[i].materialIndex]->UpdateMaterialUniforms();
			
			lastMatIndex = mSubmeshes[i].materialIndex;
		}
		// ENDTODO

		mSubmeshes[i].vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, mSubmeshes[i].indices.size(), GL_UNSIGNED_INT, (void*)mSubmeshes[i].indexOffset);
		mSubmeshes[i].vertexArray->Unbind();
	}
}

void Ivy::Mesh::CreateResources()
{
    mVertexBuffer = CreatePtr<VertexBuffer>();
    mIndexBuffer  = CreatePtr<IndexBuffer>();
}
