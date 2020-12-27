#include "ivypch.h"
#include "Mesh.h"


Ivy::UnorderedMap<Ivy::String, Ivy::Ptr<Ivy::Mesh>> Ivy::Mesh::mLoadedMeshes = {};


static inline glm::vec3 vec3_cast(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
static inline glm::vec2 vec2_cast(const aiVector3D &v) { return glm::vec2(v.x, v.y); } // it's aiVector3D because assimp's texture coordinates use that
static inline glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }

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
    //CreateResources();
}

Ivy::Mesh::Mesh(Entity* ent, String filepath, bool useMtlIfProvided) : Ivy::Component::Component(ent)
{
	//uint32_t entidx = GetEntityIndex();
	//Debug::CoreLog("entidx: {}", entidx);
	//mEnt = Scene::GetScene()->GetEntityWithIndex(entidx); //TODO: GetEntityIndex not working properly

    //CreateResources();
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

uint32_t Ivy::Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for(uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if(AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	return 0;
}

uint32_t Ivy::Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for(uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if(AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}

	return 0;
}

uint32_t Ivy::Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

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

void Ivy::Mesh::LoadBones(uint32_t meshIndex, const aiMesh * mesh)
{
	/* Load bones one by one */

	for(unsigned int i = 0; i < mesh->mNumBones; ++i)
	{
		unsigned int BoneIndex = 0;
		std::string BoneName(mesh->mBones[i]->mName.data);

		if(mBoneMapping.find(BoneName) == mBoneMapping.end())
		{
			/* allocate an index for the new bone */
			BoneIndex = mBoneCount;
			mBoneCount++;
			BoneInfo bi;
			mBoneInfo.push_back(bi);

			mBoneInfo[BoneIndex].BoneOffset = mat4_cast(mesh->mBones[i]->mOffsetMatrix);
			mBoneMapping[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = mBoneMapping[BoneName];
		}

		for(unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
		{
			//std::cout << pMesh->mBones[i]->mWeights. << std::endl;
			unsigned int VertexID = mSubmeshes[meshIndex].baseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = mesh->mBones[i]->mWeights[j].mWeight;
			mAnimatedVertices[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

void Ivy::Mesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if(pNodeAnim->mNumPositionKeys == 1)
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void Ivy::Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if(pNodeAnim->mNumRotationKeys == 1)
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void Ivy::Mesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if(pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void Ivy::Mesh::ReadNodeHierarchy(float AnimationTime, aiNode* pNode, Mat4& parentTransform)
{
	std::string NodeName(pNode->mName.data);
	aiAnimation* pAnimation = mAssimpScene->mAnimations[0];
	glm::mat4 NodeTransformation = Mat4FromAssimpMat4(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
	if(pNodeAnim)
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::vec3 scale = glm::vec3(Scaling.x, Scaling.y, Scaling.z);
		glm::mat4 ScalingM = glm::scale(glm::mat4(1.0f), scale);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		glm::quat rotation = quat_cast(RotationQ);
		glm::mat4 RotationM = glm::toMat4(rotation);
		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::vec3 translation = glm::vec3(Translation.x, Translation.y, Translation.z);
		glm::mat4 TranslationM = glm::translate(glm::mat4(1.0f), translation);
		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM *ScalingM;
	}

	// Combine with node Transformation with Parent Transformation
	glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;
	if(mBoneMapping.find(NodeName) != mBoneMapping.end())
	{
		unsigned int BoneIndex = mBoneMapping[NodeName];
		mBoneInfo[BoneIndex].FinalTransformation = mInverseTransform * GlobalTransformation * mBoneInfo[BoneIndex].BoneOffset;
	}
	for(unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

void Ivy::Mesh::BoneTransform(float time)
{
	//ReadNodeHierarchy(time, mAssimpScene->mRootNode, glm::mat4(1.0f));
	//mBoneTransforms.resize(mBoneCount);
	//for(size_t i = 0; i < mBoneCount; i++)
	//{
	//	mBoneTransforms[i] = mBoneInfo[i].FinalTransformation;
	//
	//}

	glm::mat4 Identity = glm::mat4(1.0f);
	animDuration = (float)mAssimpScene->mAnimations[0]->mDuration;

	/* Calc animation duration */
	//unsigned int numPosKeys = mAssimpScene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
	//animDuration = mAssimpScene->mAnimations[0]->mChannels[0]->mPositionKeys[numPosKeys - 1].mTime;
	//float TicksPerSecond = (float)(mAssimpScene->mAnimations[0]->mTicksPerSecond != 0 ? mAssimpScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	//float TimeInTicks = time * TicksPerSecond;
	//float AnimationTime = fmod(TimeInTicks, animDuration);
	ReadNodeHierarchy(time, mAssimpScene->mRootNode, Identity);
	mBoneTransforms.resize(mBoneCount);

	for(unsigned int i = 0; i < mBoneCount; i++)
	{
		mBoneTransforms[i] = mBoneInfo[i].FinalTransformation;
	}
}

void Ivy::Mesh::Load(String filepath, bool useMtlIfProvided)
{
	mMeshName = filepath;

	RemoveUntilCharacterInString(mMeshName, '/');
	RemoveUntilCharacterInString(mMeshName, '\\');
	RemoveFromCharacterInStringToEnd(mMeshName, '.');

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

	if(!mAssimpScene)
	{
		Debug::CoreError("Couldn't load mesh file!");
		return;
	}

	if(!mAssimpScene->HasMeshes())
	{
		Debug::CoreError("Mesh is empty/has no vertices");
		return;
	}

	mIsAnimated = mAssimpScene->mAnimations != nullptr;
	mInverseTransform = mat4_cast(scene->mRootNode->mTransformation);
	mInverseTransform = glm::inverse(mInverseTransform);

	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	mSubmeshes.reserve(mAssimpScene->mNumMeshes);

	Vector<Ptr<Material>> materials = mEntity->GetComponentsOfType<Material>();

	for(uint32_t i = 0; i < mAssimpScene->mNumMeshes; i++)
	{
		aiMesh* mesh = mAssimpScene->mMeshes[i];

		Submesh& submesh = mSubmeshes.emplace_back();
		submesh.baseVertex = vertexCount;
		submesh.baseIndex = indexCount;
		submesh.materialIndex = mesh->mMaterialIndex;
		submesh.indexCount = mesh->mNumFaces * 3;
		submesh.meshName = mesh->mName.C_Str();

		vertexCount += mesh->mNumVertices;
		indexCount += submesh.indexCount;
	}

	for(uint32_t i = 0; i < mSubmeshes.size(); i++)
	{
		const aiMesh* mesh = mAssimpScene->mMeshes[i];

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

				mAnimatedVertices.push_back(vertex);
			}

			LoadBones(i, mesh);
		}
		else
		{
			for(unsigned int j = 0; j < mesh->mNumVertices; j++)
			{
				Vertex vertex;
				vertex.position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f };
				vertex.normal = { mesh->mNormals[j].x,  mesh->mNormals[j].y,  mesh->mNormals[j].z };

				if(mesh->HasTangentsAndBitangents())
				{
					vertex.tangent = { mesh->mTangents[j].x,   mesh->mTangents[j].y,   mesh->mTangents[j].z };
					vertex.bitangent = { mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z };
				}

				if(mesh->HasTextureCoords(0))
					vertex.texcoord = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };

				mVertices.push_back(vertex);
			}
		}


		// now wak through each of the mesh's faces and retrieve the corresponding vertex indices.
		for(unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace face = mesh->mFaces[j];
			// retrieve all indices of the face and store them in the indices vector
			for(unsigned int k = 0; k < face.mNumIndices; k++)
				mIndices.push_back(face.mIndices[k]);
		}
	}


	//TraverseNodes(scene->mRootNode);

	//for(uint32_t i = 0; i < mSubmeshes.size(); i++)
	//{
	//	LoadBones(i, mAssimpScene->mMeshes[i]);
	//}


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
	//std::sort(mSubmeshes.begin(), mSubmeshes.end(), less_than_key());
}

void Ivy::Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
{
	glm::mat4 transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);
	for(uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		uint32_t mesh = node->mMeshes[i];
		auto& submesh = mSubmeshes[mesh];
		submesh.nodeName = node->mName.C_Str();
		submesh.transform = transform;
	}

	// HZ_MESH_LOG("{0} {1}", LevelToSpaces(level), node->mName.C_Str());

	for(uint32_t i = 0; i < node->mNumChildren; i++)
		TraverseNodes(node->mChildren[i], transform, level + 1);
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
			
			shader->SetUniformMat4("model", transform->getComposed() /* * mSubmeshes[i].transform*/);

			// höchstwahrscheinlich ziemlich langsam
			if(mIsAnimated)
			{
				glUniformMatrix4fv(
					glGetUniformLocation(
						shader->GetRendererID(), 
						"boneTransforms"), 
						(GLsizei)mBoneTransforms.size(), 
						GL_FALSE, 
						glm::value_ptr(mBoneTransforms[0]));
			}

		}
		// ENDTODO
		
		//TODO: Check if animated and upload animTransform as uniform mat4

		mVertexArray->Bind();
		//glDrawElements(GL_TRIANGLES, mSubmeshes[i].indices.size(), GL_UNSIGNED_INT, (void*)mSubmeshes[i].indexOffset);
		glDrawElementsBaseVertex(GL_TRIANGLES, mSubmeshes[i].indexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * mSubmeshes[i].baseIndex), mSubmeshes[i].baseVertex);
		mVertexArray->Unbind();
	}
}

void Ivy::Mesh::CreateResources()
{
    mVertexBuffer = CreatePtr<VertexBuffer>();
    mIndexBuffer  = CreatePtr<IndexBuffer>();
}

void Ivy::Mesh::SetResourceData()
{
	BufferLayout layout;
	if(mIsAnimated)
	{
		layout =
		{
			{ShaderDataType::Float4, "aPosition"},
			{ShaderDataType::Float3, "aNormal"},
			{ShaderDataType::Float3, "aTangent"},
			{ShaderDataType::Float3, "aBitangent"},
			{ShaderDataType::Float2, "aTexCoord"},
			{ShaderDataType::Int4,   "aBoneIDs"},
			{ShaderDataType::Float4, "aBoneWeights"}
		};
	}
	else
	{

		layout =
		{
			{ShaderDataType::Float4, "aPosition"},
			{ShaderDataType::Float3, "aNormal"},
			{ShaderDataType::Float3, "aTangent"},
			{ShaderDataType::Float3, "aBitangent"},
			{ShaderDataType::Float2, "aTexCoord"}
		};
	}
	
	mVertexBuffer = CreatePtr<VertexBuffer>();
	mIndexBuffer = CreatePtr<IndexBuffer>();

	mVertexArray = CreatePtr<VertexArray>(layout);
	mVertexArray->SetVertexAndIndexBuffer(mVertexBuffer, mIndexBuffer);
	mVertexArray->Bind();
	mIndexBuffer->Bind();
	if(mIsAnimated)
	{
		mVertexBuffer->SetBufferData(mAnimatedVertices.data(), mAnimatedVertices.size() * sizeof(AnimatedVertex));
	}
	else
	{
		mVertexBuffer->SetBufferData(mVertices.data(), mVertices.size() * sizeof(Vertex));
	}
	mIndexBuffer->Bind();
	mIndexBuffer->SetBufferData(mIndices.data(), mIndices.size());

	mVertexBuffer->Unbind();
	mIndexBuffer->Unbind();
	mVertexArray->Unbind();
}
