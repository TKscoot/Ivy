#pragma once
#include "Types.h"
#include "Helper.h"
#include "rendering/Buffer.h"
#include "rendering/VertexArray.h"
#include "rendering/Line.h"
#include "Component.h"
#include "scene/Entity.h"
#include "scene/components/Material.h"
#include "scene/Camera.h"
#include "core/Timer.h"

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/anim.h>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>

#include <glm/gtx/matrix_decompose.hpp>

#include <imgui.h>

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

			AABB boundingBox;
			//std::array<Line, 12> mBoundingBoxLines;

			std::string nodeName, meshName;

			float cameraDistance = 0.0f;

			bool operator<(Submesh& that)
			{
				return this->cameraDistance < that.cameraDistance;
			}
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
		struct alpha_sort_key
		{
			inline bool operator() (Submesh submesh1, Submesh submesh2)
			{
				return (submesh1.cameraDistance > submesh2.cameraDistance);
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

		/*!
		 * OnUpdate Callback override
		 * 
		 * \param deltaTime
		 */
		void OnUpdate(float deltaTime) final;

		void OnSceneLoad() final;
		void OnSceneUnload() final;

		void Destroy();

		/*!
		 * Loads a mesh from file
		 *
		 * \param filepath The path to the mesh file
		 * \param useMtlIfProvided Bool to determine if MTL Materials should be used. Default = true
		 */
		void Load();


		unsigned int getNumAnimations();
		void setAnimation(unsigned int a);
		void boneTransform(float timeInSeconds, std::vector<glm::mat4>& Transforms);
		void setBoneTransformations(GLuint shaderProgram, GLfloat currentTime);
		void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = Mat4(1.0f), uint32_t level = 0);

		void SetDrawBoundingBox(bool draw) { mDrawBoundingBox = draw; }

		bool GetDrawBoundingBox() { return mDrawBoundingBox; }

    protected:

		struct VertexBoneData
		{
			/* de pastrat numarul de elemente al IDs & Weights */
			unsigned int IDs[4];
			float Weights[4];

			VertexBoneData()
			{
				Reset();
			}

			void Reset()
			{
				for(unsigned int i = 0; i < 4; ++i)
				{
					IDs[i] = 0;
					Weights[i] = 0;
				}
			}

			void AddBoneData(unsigned int BoneID, float Weight);
		};

		// Animation Code von xphere (http://www.xphere.me/2019/05/bones-animation-with-openglassimpglm/)
		void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
		void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
		void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);


		double animDuration = 0;


		Ptr<Shader> mAABBShader = nullptr;
		Vector<GLuint> mAabbVertexArrays;
		Vector<GLuint> mAabbVertexBuffers;

		bool mNeedsAABBSetup = true;


		void DrawBoundingBox(Mat4 proj, Mat4 view);
		void SetupBoundingBox();




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
		void Draw(Ptr<Shader> shader, bool bindTextures = true);

		static float GetCameraDistance(Submesh& sm, Vec3 camPos)
		{

			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 position;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(sm.transform, scale, rotation, position, skew, perspective);

			float dist = glm::length2(position - camPos);

			return dist;
		}

		std::unique_ptr<Assimp::Importer> mImporter;

		Vector<Submesh> mSubmeshes;

		Vector<Vertex>		   mVertices;
		Vector<AnimatedVertex> mAnimatedVertices;

		Vector<uint32_t>	   mIndices;

        Ptr<VertexBuffer>      mVertexBuffer;
        Ptr<IndexBuffer>       mIndexBuffer;
						       
        Ptr<VertexArray>       mVertexArray;
						       
        BufferLayout           mBufferLayout;

		Ptr<Entity> mEnt;
		Ptr<Camera> mCamera;

		String mFilePath = "";
		bool mUseMtlIfProvided = true;
		String mMeshName = "";

		std::array<GLuint, 5> textures;

		const aiScene* mAssimpScene;

		// Animation
		bool mIsAnimated	   = false;
		
		std::map<std::string, unsigned int> mBoneMapping; // maps a bone name to its index
		unsigned int mNumBones = 0;
		std::vector<BoneInfo> mBoneInfo;
		glm::mat4 mGlobalInverseTransform;

		Vector<Mat4>		   mBoneTransforms;
		Vector<VertexBoneData> mBoneData;

		/* duration of the animation, can be changed if frames are not present in all interval */
		bool mAnimationPlaying = true;
		float mAnimationTime = 0.0f;
		float mWorldTime = 0.0f;
		float mTimeMultiplier = 1.0f;

		unsigned int mCurrentAnimation = 0;


		bool mDrawBoundingBox = false;

		Ptr<Line>  mLine = nullptr;


    };
}
