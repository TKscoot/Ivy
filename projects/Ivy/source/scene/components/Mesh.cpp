#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/cimport.h>

Ivy::Mesh::Mesh()
{
    CreateResources();
}

Ivy::Mesh::Mesh(String filepath)
{
    CreateResources();
    Load(filepath);
}

Ivy::Mesh::Mesh(Vector<Vertex> vertices, Vector<uint32_t> indices)
{
}

void Ivy::Mesh::Load(String filepath)
{
    uint32_t flags =
        aiProcess_GenUVCoords |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs |
        aiProcess_Triangulate ;
        //aiProcess_MakeLeftHanded |
        //aiProcess_FixInfacingNormals ;
        //aiProcess_FlipWindingOrder;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filepath.c_str(), flags);

    if(!scene)
    {
        Debug::CoreError("Couldn't load mesh file!");
        return;
    }

    if(!scene->HasMeshes())
    {
        Debug::CoreError("Mesh is empty/has no vertices");
        return;
    }

    uint32_t totalVertexCount = 0;
    uint32_t totalIndexCount  = 0;

    for(uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        totalVertexCount += mesh->mNumVertices;
        totalIndexCount  += mesh->mNumFaces * 3;
    }

    mVertices.resize(totalVertexCount);
    mIndices.resize(totalIndexCount);

    uint32_t currentVertexOffset = 0;
    uint32_t currentIndexOffset  = 0;

    for(uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        for(uint32_t j = 0; j < mesh->mNumVertices; j++)
        {
            mVertices[j].position = mesh->HasBones() ? Vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f):Vec4(0.0f);
            mVertices[j].normal   = mesh->HasNormals() ? Vec3(mesh->mNormals[j].x,  mesh->mNormals[j].y,  mesh->mNormals[j].z) : Vec3(0.0f);
            mVertices[j].tangent  = mesh->HasTangentsAndBitangents() ? Vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z) : Vec3(0.0f);
            mVertices[j].texcoord = mesh->HasTextureCoords(0) ? Vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y) : Vec2(0.0f);
        }

        for(uint32_t j = 0; j < mesh->mNumFaces; j++)
        {
            aiFace* face = mesh->mFaces + j;

            for(uint32_t k = 0; k < face->mNumIndices; k++)
            {
                mIndices[((3*j)+k)] =(*(face->mIndices +k));
            }
        }
        
        currentVertexOffset = mesh->mNumVertices;
        currentIndexOffset = mesh->mNumFaces * 3;
    }

    mVertexBuffer->SetBufferData(mVertices.data(), mVertices.size());
    mIndexBuffer->SetBufferData(mIndices.data(), mIndices.size());
    mVertexArray->SetIndexBuffer(mIndexBuffer);
    mVertexArray->SetVertexBuffer(mVertexBuffer);
}

void Ivy::Mesh::Draw()
{
    mVertexArray->Bind();
    glDrawElements(GL_TRIANGLES, mIndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}

void Ivy::Mesh::CreateResources()
{
    mVertexBuffer = CreatePtr<VertexBuffer>();
    mIndexBuffer  = CreatePtr<IndexBuffer>();

    BufferLayout layout =
    {
        {ShaderDataType::Float4, "aPosition"},
        {ShaderDataType::Float3, "aNormal"},
        {ShaderDataType::Float3, "aTangent"},
        {ShaderDataType::Float2, "aTexCoord"}
    };

    mVertexBuffer->SetLayout(layout);

    // Set Index- and Vertexbuffer in VertexArray
    mVertexArray = CreatePtr<VertexArray>();
    mVertexArray->SetVertexBuffer(mVertexBuffer);
    mVertexArray->SetIndexBuffer(mIndexBuffer);
    mVertexArray->Unbind(); // does not need to be unbound but it's good practice to do so
}

