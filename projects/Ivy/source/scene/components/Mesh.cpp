#include "ivypch.h"
#include "Mesh.h"


Ivy::UnorderedMap<Ivy::String, Ivy::Ptr<Ivy::Mesh>> Ivy::Mesh::mLoadedMeshes = {};

Ivy::Mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix)
{
	Ivy::Mat4 result;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
	result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
	result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
	result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
	return result;
}

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

void Ivy::Mesh::OnUpdate(float deltaTime)
{
	if(mIsAnimated)
	{
		if(mAnimationPlaying)
		{
			mWorldTime += deltaTime;

			float ticksPerSecond = (float)(mAssimpScene->mAnimations[0]->mTicksPerSecond != 0 ? mAssimpScene->mAnimations[0]->mTicksPerSecond : 25.0f) * mTimeMultiplier;
			mAnimationTime += deltaTime * ticksPerSecond;
			mAnimationTime = fmod(mAnimationTime, (float)mAssimpScene->mAnimations[0]->mDuration);
		}

		// TODO: We only need to recalc bones if rendering has been requested at the current animation frame
		BoneTransform(mAnimationTime);
	}
}

Ivy::Vec3 Ivy::Mesh::InterpolateTranslation(float animationTime, aiNodeAnim* nodeAnim)
{
	if(nodeAnim->mNumPositionKeys == 1)
	{
		// No interpolation necessary for single value
		auto v = nodeAnim->mPositionKeys[0].mValue;
		return { v.x, v.y, v.z };
	}

	uint32_t PositionIndex = FindPosition(animationTime, nodeAnim);
	uint32_t NextPositionIndex = (PositionIndex + 1);

	float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	Factor = glm::clamp(Factor, 0.0f, 1.0f);
	const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	auto aiVec = Start + Factor * Delta;
	return { aiVec.x, aiVec.y, aiVec.z };
}

uint32_t Ivy::Mesh::FindPosition(float AnimationTime, aiNodeAnim* pNodeAnim)
{
	for(uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if(AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	return 0;
}


uint32_t Ivy::Mesh::FindRotation(float AnimationTime, aiNodeAnim* pNodeAnim)
{
	for(uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if(AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}

	return 0;
}


uint32_t Ivy::Mesh::FindScaling(float AnimationTime, aiNodeAnim* pNodeAnim)
{
	for(uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if(AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
			return i;
	}

	return 0;
}

Ivy::Quat Ivy::Mesh::InterpolateRotation(float animationTime, aiNodeAnim* nodeAnim)
{
	if(nodeAnim->mNumRotationKeys == 1)
	{
		// No interpolation necessary for single value
		auto v = nodeAnim->mRotationKeys[0].mValue;
		return glm::quat(v.w, v.x, v.y, v.z);
	}

	uint32_t RotationIndex = FindRotation(animationTime, nodeAnim);
	uint32_t NextRotationIndex = (RotationIndex + 1);
	float DeltaTime = (float)(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	Factor = glm::clamp(Factor, 0.0f, 1.0f);
	const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
	auto q = aiQuaternion();
	aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
	q = q.Normalize();
	return glm::quat(q.w, q.x, q.y, q.z);
}


Ivy::Vec3 Ivy::Mesh::InterpolateScale(float animationTime, aiNodeAnim* nodeAnim)
{
	if(nodeAnim->mNumScalingKeys == 1)
	{
		// No interpolation necessary for single value
		auto v = nodeAnim->mScalingKeys[0].mValue;
		return { v.x, v.y, v.z };
	}

	uint32_t index = FindScaling(animationTime, nodeAnim);
	uint32_t nextIndex = (index + 1);
	float deltaTime = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
	float factor = (animationTime - (float)nodeAnim->mScalingKeys[index].mTime) / deltaTime;
	factor = glm::clamp(factor, 0.0f, 1.0f);
	const auto& start = nodeAnim->mScalingKeys[index].mValue;
	const auto& end = nodeAnim->mScalingKeys[nextIndex].mValue;
	auto delta = end - start;
	auto aiVec = start + factor * delta;
	return { aiVec.x, aiVec.y, aiVec.z };
}

aiNodeAnim* Ivy::Mesh::FindNodeAnim(aiAnimation* animation, std::string& nodeName)
{
	for(uint32_t i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[i];
		if(std::string(nodeAnim->mNodeName.data) == nodeName)
			return nodeAnim;
	}
	return nullptr;
}

void Ivy::Mesh::ReadNodeHierarchy(float AnimationTime, aiNode* pNode, Mat4& parentTransform)
{
	std::string name(pNode->mName.data);
	aiAnimation* animation = mAssimpScene->mAnimations[0];
	glm::mat4 nodeTransform(Mat4FromAssimpMat4(pNode->mTransformation));
	aiNodeAnim* nodeAnim = FindNodeAnim(animation, name);

	if(nodeAnim)
	{
		glm::vec3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

		glm::quat rotation = InterpolateRotation(AnimationTime, nodeAnim);
		glm::mat4 rotationMatrix = glm::toMat4(rotation);

		glm::vec3 scale = InterpolateScale(AnimationTime, nodeAnim);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

		nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
	}

	glm::mat4 transform = parentTransform * nodeTransform;

	if(mBoneMapping.find(name) != mBoneMapping.end())
	{
		uint32_t BoneIndex = mBoneMapping[name];
		mBoneInfo[BoneIndex].FinalTransformation = mInverseTransform * transform * mBoneInfo[BoneIndex].BoneOffset;
	}

	for(uint32_t i = 0; i < pNode->mNumChildren; i++)
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
}

void Ivy::Mesh::BoneTransform(float time)
{
	ReadNodeHierarchy(time, mAssimpScene->mRootNode, glm::mat4(1.0f));
	mBoneTransforms.resize(mBoneCount);
	for(size_t i = 0; i < mBoneCount; i++)
		mBoneTransforms[i] = mBoneInfo[i].FinalTransformation;
}

void Ivy::Mesh::Load(String filepath, bool useMtlIfProvided)
{
	mMeshName = filepath;

	RemoveUntilCharacterInString(    mMeshName, '/' );
	RemoveUntilCharacterInString(    mMeshName, '\\');
	RemoveFromCharacterInStringToEnd(mMeshName, '.' );

	uint32_t flags =
		aiProcess_CalcTangentSpace |       
		aiProcess_Triangulate |            
		aiProcess_SortByPType |            
		aiProcess_GenNormals |             
		aiProcess_GenUVCoords |            
		aiProcess_OptimizeMeshes |         
		aiProcess_ValidateDataStructure;   


	mImporter = std::make_unique<Assimp::Importer>();
	const aiScene* scene = mImporter->ReadFile(filepath.c_str(), flags);
	mAssimpScene = scene;

	if (!mAssimpScene)
	{
		Debug::CoreError("Couldn't load mesh file!");
		return;
	}

	if (!mAssimpScene->HasMeshes())
	{
		Debug::CoreError("Mesh is empty/has no vertices");
		return;
	}

	mIsAnimated = mAssimpScene->mAnimations != nullptr;
	mInverseTransform = glm::inverse(Mat4FromAssimpMat4(scene->mRootNode->mTransformation));

	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	mSubmeshes.resize(mAssimpScene->mNumMeshes);

	//for (uint32_t i = 0; i < mAssimpScene->mNumMeshes; i++)
	//{
	//	aiMesh* mesh = mAssimpScene->mMeshes[i];
	//
	//	totalVertexCount += mesh->mNumVertices;
	//	totalIndexCount += mesh->mNumFaces * 3;
	//}
	//
	//mVertexBuffer->Bind();
	//if(mIsAnimated)
	//{
	//	mVertexBuffer->SetBufferData(nullptr, totalVertexCount * sizeof(AnimatedVertex));
	//}
	//else
	//{
	//	mVertexBuffer->SetBufferData(nullptr, totalVertexCount * sizeof(Vertex));
	//}
	//mIndexBuffer->Bind();
	//mIndexBuffer->SetBufferData(nullptr, totalIndexCount);

	uint32_t currentVertexOffset = 0;
	uint32_t currentIndexOffset = 0;

	Vector<Ptr<Material>> materials = mEntity->GetComponentsOfType<Material>();

	for (uint32_t i = 0; i < mAssimpScene->mNumMeshes; i++)
	{
		aiMesh* mesh = mAssimpScene->mMeshes[i];

		Submesh& submesh = mSubmeshes[i];

		submesh.vertices.resize(mesh->mNumVertices);
		submesh.baseVertex = currentVertexOffset; //TODO: Eventuell buggy
		submesh.index = i;
		submesh.materialIndex = mesh->mMaterialIndex;

		Vector<Vertex> vertices;
		Vector<unsigned int> indices;

		// walk through each of the mesh's vertices
		if(mIsAnimated)
		{
			for(size_t j = 0; j < mesh->mNumVertices; j++)
			{
				AnimatedVertex vertex;
				vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f };
				vertex.normal = { mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };
				
				if(mesh->HasTangentsAndBitangents())
				{
					vertex.tangent = { mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z };
					vertex.bitangent = { mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z };
				}
				
				if(mesh->HasTextureCoords(0))
					vertex.texcoord = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };

				submesh.animatedVertices.push_back(vertex);
			}
			// now wak through each of the mesh's faces and retrieve the corresponding vertex indices.
			for(unsigned int j = 0; j < mesh->mNumFaces; j++)
			{
				aiFace face = mesh->mFaces[j];
				// retrieve all indices of the face and store them in the indices vector
				for(unsigned int k = 0; k < face.mNumIndices; k++)
					submesh.indices.push_back(face.mIndices[k]);
			}

			submesh.vertexOffset = currentVertexOffset * sizeof(AnimatedVertex);
			submesh.indexOffset = currentIndexOffset * sizeof(uint32_t);

			currentVertexOffset += submesh.animatedVertices.size();
			currentIndexOffset += submesh.indices.size();
		}
		else
		{

			for(unsigned int j = 0; j < mesh->mNumVertices; j++)
			{
				Vertex vertex;
				vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f };
				vertex.normal   = { mesh->mNormals[j].x,  mesh->mNormals[j].y,  mesh->mNormals[j].z };

				if(mesh->HasTangentsAndBitangents())
				{
					vertex.tangent   = { mesh->mTangents[j].x,   mesh->mTangents[j].y,   mesh->mTangents[j].z };
					vertex.bitangent = { mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z };
				}

				if(mesh->HasTextureCoords(0))
					vertex.texcoord = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };

				vertices.push_back(vertex);
			}
			// now wak through each of the mesh's faces and retrieve the corresponding vertex indices.
			for(unsigned int j = 0; j < mesh->mNumFaces; j++)
			{
				aiFace face = mesh->mFaces[j];
				// retrieve all indices of the face and store them in the indices vector
				for(unsigned int k = 0; k < face.mNumIndices; k++)
					indices.push_back(face.mIndices[k]);
			}

			submesh.vertices = vertices;
			submesh.indices = indices;


			submesh.vertexOffset = currentVertexOffset * sizeof(Vertex);
			submesh.indexOffset  = currentIndexOffset * sizeof(uint32_t);

			currentVertexOffset += submesh.vertices.size();
			currentIndexOffset  += submesh.indices.size();
		
		}
	}

	TraverseNodes(scene->mRootNode);


	//bones
	if(mIsAnimated)
	{
		for(size_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];
			Submesh& submesh = mSubmeshes[m];

			for(size_t i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* bone = mesh->mBones[i];
				std::string boneName(bone->mName.data);
				int boneIndex = 0;

				if(mBoneMapping.find(boneName) == mBoneMapping.end())
				{
					// Allocate an index for a new bone
					boneIndex = mBoneCount;
					mBoneCount++;
					BoneInfo bi;
					mBoneInfo.push_back(bi);
					mBoneInfo[boneIndex].BoneOffset = Mat4FromAssimpMat4(bone->mOffsetMatrix);
					mBoneMapping[boneName] = boneIndex;
				}
				else
				{
					Debug::CoreWarning("Found existing bone in map");
					boneIndex = mBoneMapping[boneName];
				}

				for(size_t j = 0; j < bone->mNumWeights; j++)
				{
					int VertexID = submesh.baseVertex + bone->mWeights[j].mVertexId;
					float Weight = bone->mWeights[j].mWeight;
					submesh.animatedVertices[VertexID].AddBoneData(boneIndex, Weight);
				}
			}
		}
	}
	
	SetResourceData();

	//material assignment
	if(useMtlIfProvided)
	{

		if(mAssimpScene->HasMaterials())
		{
			for(int i = 1; i < mAssimpScene->mNumMaterials; i++)
			{

				materials.push_back(mEntity->AddComponent(CreatePtr<Material>()));
			}


		}
		

		for(uint32_t i = 0; i < mAssimpScene->mNumMeshes; i++)
		{
			aiMesh* mesh = mAssimpScene->mMeshes[i];

			// Get the material
			aiMaterial* assimpMaterial = mAssimpScene->mMaterials[mesh->mMaterialIndex];
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

	if(mIsAnimated)
	{
		Vector<Ptr<Material>> mats = mEntity->GetComponentsOfType<Material>();
		for(int i = 0; i < mats.size(); i++)
		{
			mats[i]->SetShader(CreatePtr<Shader>("shaders/Default_Animated.vert", "shaders/PBR.frag"));
		}

	}
	
	// sorting submeshes for better texture binding performance
	std::sort(mSubmeshes.begin(), mSubmeshes.end(), less_than_key());
}

void Ivy::Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
{
	glm::mat4 transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);
	for(uint32_t i = 0; i < node->mNumMeshes; i++)
	{ 
		uint32_t mesh = node->mMeshes[i];
		mSubmeshes[mesh].transform = transform;
	} 

	// HZ_MESH_LOG("{0} {1}", LevelToSpaces(level), node->mName.C_Str());

	for(uint32_t i = 0; i < node->mNumChildren; i++)
	{
		TraverseNodes(node->mChildren[i], transform, level + 1);
	}
}

void Ivy::Mesh::Draw(bool bindTextures)
{
	if (!IsActive()) return;

	auto& materials = mEntity->GetComponentsOfType<Material>();
	auto& transform = mEntity->GetFirstComponentOfType<Transform>();

	for (int i = 0; i < mSubmeshes.size(); i++)
	{

		// TODO: VERY PERFORMANCE HUNGRY WHEN LOTS OF TEXTUREBINDS: FIX
		if (bindTextures && materials.size() >= mSubmeshes[i].materialIndex)
		{
			for (auto& kv : materials[mSubmeshes[i].materialIndex]->GetTextures())
			{
				textures[static_cast<uint32_t>(kv.first)] = kv.second->GetRendererID();
			}

			glBindTextures(0, textures.size(), textures.data());

			Ptr<Shader> shader = materials[mSubmeshes[i].materialIndex]->GetShader();
			shader->Bind();
			materials[mSubmeshes[i].materialIndex]->UpdateShaderTextureBools();
			materials[mSubmeshes[i].materialIndex]->UpdateMaterialUniforms();
			
			shader->SetUniformMat4("model", transform->getComposed()/* * mSubmeshes[i].transform*/);

			// höchstwahrscheinlich ziemlich langsam
			if(mIsAnimated)
			{
				for(size_t i = 0; i < mBoneTransforms.size(); i++)
				{
					std::string uniformName = std::string("boneTransforms[") + std::to_string(i) + std::string("]");
					shader->SetUniformMat4(uniformName, mBoneTransforms[i]);
				}
			}

		}
		// ENDTODO
		
		//TODO: Check if animated and upload animTransform as uniform mat4

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

void Ivy::Mesh::SetResourceData()
{
	if(mIsAnimated)
	{
		for(int i = 0; i < mSubmeshes.size(); i++)
		{
			Submesh& submesh = mSubmeshes[i];
			BufferLayout layout =
			{
				{ShaderDataType::Float4, "aPosition",	 submesh.vertexOffset},
				{ShaderDataType::Float3, "aNormal"  ,	 submesh.vertexOffset},
				{ShaderDataType::Float3, "aTangent" ,	 submesh.vertexOffset},
				{ShaderDataType::Float3, "aBitangent",	 submesh.vertexOffset},
				{ShaderDataType::Float2, "aTexCoord",	 submesh.vertexOffset},
				{ShaderDataType::Int4,   "aBoneIDs",	 submesh.vertexOffset},
				{ShaderDataType::Float4, "aBoneWeights", submesh.vertexOffset}

			};

			submesh.vertexArray = CreatePtr<VertexArray>(layout);
			submesh.vertexArray->SetVertexAndIndexBuffer(mVertexBuffer, mIndexBuffer);
			submesh.vertexArray->Bind();

			submesh.vertexArray->GetVertexBuffer()->Bind();
			submesh.vertexArray->GetVertexBuffer()->SetBufferSubData(submesh.vertexOffset, submesh.animatedVertices.data(), submesh.animatedVertices.size() * sizeof(AnimatedVertex));
			submesh.vertexArray->GetIndexBuffer()->Bind();
			submesh.vertexArray->GetIndexBuffer()->SetBufferSubData(submesh.indexOffset, submesh.indices.data(), submesh.indices.size());

			mVertexBuffer->Unbind();
			mIndexBuffer->Unbind();
			submesh.vertexArray->Unbind();
		}

	}
	else
	{
		for(int i = 0; i < mSubmeshes.size(); i++)
		{
			Submesh& submesh = mSubmeshes[i];
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
		}
	}
}
