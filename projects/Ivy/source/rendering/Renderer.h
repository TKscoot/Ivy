#pragma once
#include "Types.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "scene/components/Mesh.h"
#include "scene/components/Transform.h"
#include "scene/components/Material.h"
#include "scene/Scene.h"
#include "Texture.h"
#include "scene/Camera.h"


namespace Ivy
{
	class Renderer
	{
	public:
		Renderer(Ptr<Window> window)
			: mWindow(window)
		{}

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
		Ptr<Window>       mWindow;
		Ptr<VertexArray>  mVertexArray;
		Ptr<VertexBuffer> mVertexBuffer;
		Ptr<IndexBuffer>  mIndexBuffer;
		Ptr<Shader>		  mShader;
        Ptr<Mesh>         mMesh;
		Ptr<Texture2D>    mTexture;
		Ptr<Texture2D>    mTexture1;
		Ptr<Camera>       mCamera;
		Ptr<Scene>		  mScene;

	};
}
