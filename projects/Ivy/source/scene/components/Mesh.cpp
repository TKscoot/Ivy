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
	mCamera = ent->GetSceneCamera();
}

Ivy::Mesh::Mesh(Entity* ent, String filepath, bool useMtlIfProvided) : Ivy::Component::Component(ent)
{
	//uint32_t entidx = GetEntityIndex();
	//Debug::CoreLog("entidx: {}", entidx);
	//mEnt = Scene::GetScene()->GetEntityWithIndex(entidx); //TODO: GetEntityIndex not working properly

    //CreateResources();
	mCamera = ent->GetSceneCamera();

    Load(filepath, useMtlIfProvided);
}

Ivy::Mesh::Mesh(Entity* ent, Vector<Vertex> vertices, Vector<uint32_t> indices) : Ivy::Component::Component(ent)
{
	//mEnt = Scene::GetScene()->GetEntityWithIndex(GetEntityIndex());
	mCamera = ent->GetSceneCamera();

}

void Ivy::Mesh::OnUpdate(float deltaTime)
{
	if(mIsAnimated)
	{
		ImGui::Begin("Mesh");
		ImGui::LabelText("", mAssimpScene->mAnimations[mCurrentAnimation]->mName.C_Str());
		ImGui::Checkbox("Play animation", &mAnimationPlaying);
		ImGui::End();
		if(mAnimationPlaying)
		{
			mWorldTime += deltaTime;
		
			float ticksPerSecond = (float)(mAssimpScene->mAnimations[mCurrentAnimation]->mTicksPerSecond != 0 ? mAssimpScene->mAnimations[mCurrentAnimation]->mTicksPerSecond : 25.0f) * mTimeMultiplier;
			mAnimationTime += deltaTime * ticksPerSecond;
			mAnimationTime = fmod(mAnimationTime, (float)mAssimpScene->mAnimations[mCurrentAnimation]->mDuration);
		}

		// TODO: We only need to recalc bones if rendering has been requested at the current animation frame
		boneTransform(mAnimationTime, mBoneTransforms);
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
	mGlobalInverseTransform = mat4_cast(scene->mRootNode->mTransformation);
	mGlobalInverseTransform = glm::inverse(mGlobalInverseTransform);

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


	TraverseNodes(scene->mRootNode);

	mBoneData.resize(vertexCount);
	for(uint32_t i = 0; i < mSubmeshes.size(); i++)
	{
		LoadBones(i, mAssimpScene->mMeshes[i], mBoneData);
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
	//std::sort(mSubmeshes.begin(), mSubmeshes.end(), less_than_key());

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
			
			shader->SetUniformMat4("model", transform->getComposed());

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
		

		mVertexArray->Bind();
		glDrawElementsBaseVertex(
			GL_TRIANGLES, 
			mSubmeshes[i].indexCount, 
			GL_UNSIGNED_INT, 
			(void*)(sizeof(uint32_t) * mSubmeshes[i].baseIndex), 
			mSubmeshes[i].baseVertex);
		mVertexArray->Unbind();
	}
}

void Ivy::Mesh::Draw(Ptr<Shader> shader, bool bindTextures)
{
	if(!IsActive()) return;

	auto& materials = mEntity->GetComponentsOfType<Material>();
	auto& transform = mEntity->GetFirstComponentOfType<Transform>();

	for(int i = 0; i < mSubmeshes.size(); i++)
	{

		// TODO: VERY PERFORMANCE HUNGRY WHEN LOTS OF TEXTUREBINDS: FIX
		if(bindTextures && materials.size() >= mSubmeshes[i].materialIndex)
		{
			for(auto& kv : materials[mSubmeshes[i].materialIndex]->GetTextures())
			{
				textures[static_cast<uint32_t>(kv.first)] = kv.second->GetRendererID();
			}

			glBindTextures(0, textures.size(), textures.data());

			//Ptr<Shader> shader = materials[mSubmeshes[i].materialIndex]->GetShader();
			shader->Bind();
			materials[mSubmeshes[i].materialIndex]->UpdateShaderTextureBools();
			materials[mSubmeshes[i].materialIndex]->UpdateMaterialUniforms();

			shader->SetUniformMat4("model", transform->getComposed());



		}
		// ENDTODO

		if(mIsAnimated)
		{
			shader->SetUniformInt("isAnimated", true);
			glUniformMatrix4fv(
				glGetUniformLocation(
					shader->GetRendererID(),
					"boneTransforms"),
					(GLsizei)mBoneTransforms.size(),
				GL_FALSE,
				glm::value_ptr(mBoneTransforms[0]));
		}
		else
		{
			shader->SetUniformInt("isAnimated", false);
		}

		mVertexArray->Bind();
		glDrawElementsBaseVertex(
			GL_TRIANGLES,
			mSubmeshes[i].indexCount,
			GL_UNSIGNED_INT,
			(void*)(sizeof(uint32_t) * mSubmeshes[i].baseIndex),
			mSubmeshes[i].baseVertex);
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
		GLuint boneBuffer = 0;
		glGenBuffers(1, &boneBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, boneBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mBoneData[0]) * mBoneData.size(), &mBoneData[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
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

// Animation Code von xphere (http://www.xphere.me/2019/05/bones-animation-with-openglassimpglm/)
void Ivy::Mesh::setBoneTransformations(GLuint shaderProgram, GLfloat currentTime)
{
	std::vector<glm::mat4> Transforms;
	boneTransform((float)currentTime, Transforms);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "gBones"), (GLsizei)Transforms.size(), GL_FALSE, glm::value_ptr(Transforms[0]));
}

/* Adds a new Bone */
void Ivy::Mesh::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for(unsigned int i = 0; i < 4; ++i)
		if(Weights[i] == 0.0)
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
}

unsigned int Ivy::Mesh::getNumAnimations()
{
	return mAssimpScene->mNumAnimations;
}

void Ivy::Mesh::setAnimation(unsigned int a)
{
	if(a >= 0 && a < getNumAnimations())
		mCurrentAnimation = a;
}

void Ivy::Mesh::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
	/* Load bones one by one */

	for(unsigned int i = 0; i < pMesh->mNumBones; ++i)
	{
		unsigned int BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if(mBoneMapping.find(BoneName) == mBoneMapping.end())
		{
			/* allocate an index for the new bone */
			BoneIndex = mNumBones;
			mNumBones++;
			BoneInfo bi;
			mBoneInfo.push_back(bi);

			mBoneInfo[BoneIndex].BoneOffset = mat4_cast(pMesh->mBones[i]->mOffsetMatrix);
			mBoneMapping[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = mBoneMapping[BoneName];
		}

		for(unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; ++j)
		{
			//std::cout << pMesh->mBones[i]->mWeights. << std::endl;
			unsigned int VertexID = mSubmeshes[MeshIndex].baseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			mBoneData[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

unsigned int Ivy::Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for(unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{

		if(AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

unsigned int Ivy::Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for(unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if(AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

unsigned int Ivy::Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for(unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if(AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
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

void Ivy::Mesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = mAssimpScene->mAnimations[mCurrentAnimation];

	glm::mat4 NodeTransformation = mat4_cast(pNode->mTransformation);

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
	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if(mBoneMapping.find(NodeName) != mBoneMapping.end())
	{
		unsigned int BoneIndex = mBoneMapping[NodeName];
		mBoneInfo[BoneIndex].FinalTransformation = mGlobalInverseTransform * GlobalTransformation * mBoneInfo[BoneIndex].BoneOffset;
	}

	for(unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

void Ivy::Mesh::boneTransform(float timeInSeconds, std::vector<glm::mat4>& Transforms)
{
	glm::mat4 Identity = glm::mat4(1.0f);

	/* Calc animation duration */
	unsigned int numPosKeys = mAssimpScene->mAnimations[mCurrentAnimation]->mChannels[0]->mNumPositionKeys;
	animDuration = mAssimpScene->mAnimations[mCurrentAnimation]->mChannels[0]->mPositionKeys[numPosKeys - 1].mTime;

	float TicksPerSecond = (float)(mAssimpScene->mAnimations[mCurrentAnimation]->mTicksPerSecond != 0 ? mAssimpScene->mAnimations[mCurrentAnimation]->mTicksPerSecond : 25.0f);
	//TicksPerSecond = 3;
	float TimeInTicks = timeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, animDuration);

	ReadNodeHeirarchy(timeInSeconds, mAssimpScene->mRootNode, Identity);

	Transforms.resize(mNumBones);

	for(unsigned int i = 0; i < mNumBones; i++)
	{
		Transforms[i] = mBoneInfo[i].FinalTransformation;
	}
}

const aiNodeAnim* Ivy::Mesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for(unsigned int i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if(std::string(pNodeAnim->mNodeName.data) == NodeName)
		{
			return pNodeAnim;
		}
	}

	return NULL;
}
// Animation Code von xphere END

void Ivy::Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
{
	glm::mat4 transform = /*parentTransform **/ Mat4FromAssimpMat4(node->mTransformation);

	for(uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		uint32_t mesh = node->mMeshes[i];
		auto& submesh = mSubmeshes[mesh];
		submesh.transform = transform;
	}

	for(uint32_t i = 0; i < node->mNumChildren; i++)
		TraverseNodes(node->mChildren[i], transform, level + 1);
}