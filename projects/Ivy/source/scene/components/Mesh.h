#pragma once
#include "Types.h"
#include "Helper.h"
#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"
#include "Component.h"
#include "scene/Entity.h"
#include "scene/components/Material.h"

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/anim.h>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>

//struct aiNode;
//struct aiAnimation;
//struct aiNodeAnim;
//struct aiScene;

//namespace Assimp
//{
//	class Importer;
//}

class Renderer;

namespace Ivy
{
	/*!
	 * Class for loading meshes and their materials (if provided)
	 * 
	 */
    class Mesh : public Component
    {
    public:
        friend class Renderer;
		friend class Scene;
		friend class ShadowRenderPass;
		friend class SceneRenderPass;

		/*!
		 * Submesh data
		 * 
		 */
		struct Submesh
		{
			uint32_t baseVertex;
			uint32_t baseIndex;
			uint32_t materialIndex;
			uint32_t indexCount;

			glm::mat4 transform;

			std::string nodeName, meshName;
		};

		/*!
		 * Internal
		 * used for submesh sorting
		 * 
		 */
		struct less_than_key
		{
			inline bool operator() (const Submesh& struct1, const Submesh& struct2)
			{
				return (struct1.materialIndex < struct2.materialIndex);
			}
		};
		
		/*!
		 * Default Mesh constructor
		 * 
		 * \param ent The entity of this attached mesh component
		 */
        Mesh(Entity* ent);

		/*!
		 * Loads a mesh from file
		 * 
		 * \param ent The entity of this attached mesh component
		 * \param filepath The path to the mesh file
		 * \param useMtlIfProvided Bool to determine if MTL Materials should be used. Default = true
		 */
        Mesh(Entity* ent, String filepath, bool useMtlIfProvided = true);
		
		/*!
		 * Creates a mesh from vertices and indices
		 * 
		 * \param ent The entity of this attached mesh component
		 * \param vertices The vertex data of the mesh
		 * \param indices The index data of the mesh
		 */
        Mesh(Entity* ent, Vector<Vertex> vertices, Vector<uint32_t> indices);

		void OnUpdate(float deltaTime) final;

		/*!
		 * Loads a mesh from file
		 *
		 * \param filepath The path to the mesh file
		 * \param useMtlIfProvided Bool to determine if MTL Materials should be used. Default = true
		 */
		void Load(String filepath, bool useMtlIfProvided = true);

    protected:
		//Animation
		void LoadBones(uint32_t meshIndex, const aiMesh* mesh);

		void ReadNodeHierarchy(float AnimationTime, aiNode* pNode, Mat4& parentTransform);
		void BoneTransform(float time);

		aiNodeAnim* FindNodeAnim(aiAnimation* animation, std::string& nodeName);
		uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindScaling( float AnimationTime, const aiNodeAnim* pNodeAnim);
		Vec3 InterpolateTranslation(float animationTime, aiNodeAnim* nodeAnim);
		Quat InterpolateRotation(float animationTime, aiNodeAnim* nodeAnim);
		Vec3 InterpolateScale(float animationTime, aiNodeAnim* nodeAnim);
		std::vector<Mat4>& GetBoneTransforms() { return mBoneTransforms; }
		void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);


		void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

		double animDuration = 0;



		struct LogStream : public Assimp::LogStream
		{
			static void Initialize()
			{
				if(Assimp::DefaultLogger::isNullLogger())
				{
					Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
					Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn | Assimp::Logger::Info | Assimp::Logger::Debugging);
				}
			}

			virtual void write(const char* message) override
			{
				Debug::CoreLog("Assimp: {0}", message);
			}
		};

        void CreateResources();
		void SetResourceData();
        void Draw(bool bindTextures = true);

		std::unique_ptr<Assimp::Importer> mImporter;

		static UnorderedMap<String, Ivy::Ptr<Mesh>> mLoadedMeshes;

		Vector<Submesh> mSubmeshes;
		UnorderedMap<String, Ivy::Ptr<Texture2D>> mLoadedTextures;


		Vector<Vertex>		   mVertices;
		Vector<AnimatedVertex> mAnimatedVertices;

		Vector<uint32_t>	   mIndices;

        Ptr<VertexBuffer>      mVertexBuffer;
        Ptr<IndexBuffer>       mIndexBuffer;
						       
        Ptr<VertexArray>       mVertexArray;
						       
        BufferLayout           mBufferLayout;

		Ptr<Entity> mEnt;

		String mMeshName;

		std::array<GLuint, 5> textures;

		const aiScene* mAssimpScene;
		// Animation
		bool mIsAnimated	   = false;
		bool mAnimationPlaying = true;
		Mat4 mInverseTransform = Mat4(0);
		
		uint32_t mBoneCount = 0;
		std::vector<BoneInfo> mBoneInfo;
		std::unordered_map<String, uint32_t> mBoneMapping;
		std::vector<Mat4> mBoneTransforms;

		float mAnimationTime  = 0.0f;
		float mWorldTime	  = 0.0f;
		float mTimeMultiplier = 1.0f;

    };
}
