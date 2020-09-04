#pragma once
#include "Types.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "scene/components/Mesh.h"
#include "Texture.h"
#include "scene/components/Transform.h"

namespace Ivy
{
	class Renderer
	{
	public:

		void Initialize();

		void Render();

		static void GLLogCallback(
								  unsigned source,
								  unsigned type,
								  unsigned id,
								  unsigned severity,
								  int length,
								  const char* message,
								  const void* userParam);

	private:

		// Methods
		void EnableDebugMessages();


		// Variables
		Ptr<VertexArray>  mVertexArray;
		Ptr<VertexBuffer> mVertexBuffer;
		Ptr<IndexBuffer>  mIndexBuffer;
		Ptr<Shader>	  mShader;
                Ptr<Mesh>         mMesh;
		Ptr<Texture2D>    mTexture;
		Ptr<Texture2D>    mTexture1;
                Transform    mTransform;
		GLuint            mVertexArrayObject = 0;
		GLuint            mShaderProgram = 0;

	};
}
