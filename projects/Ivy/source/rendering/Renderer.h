#pragma once
#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"

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
		Ptr<Shader>		  mShader;
		GLuint            mVertexArrayObject = 0;
		GLuint            mShaderProgram = 0;

	};
}