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
		/*!
		 * Renderer constructor
		 * 
		 * \param window the window you want to render to
		 */
		Renderer(Ptr<Window> window)
			: mWindow(window)
		{}

		/*!
		 * Initializes the renderer
		 * 
		 * (Don't worry. The engine takes care of this)
		 * 
		 */
		void Initialize();

		/*!
		 * Renders a frame
		 * 
		 * (Don't worry. The engine takes care of this)
		 *
		 * \param deltaTime delta time in ms
		 */
		void Render(float deltaTime);

		/*!
		 * Callback of the OpenGL logs
		 *
		 *  (Don't worry. The engine takes care of this)
		 *
		 * \param source
		 * \param type
		 * \param id
		 * \param severity
		 * \param length
		 * \param message
		 * \param userParam
		 */
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
		Ptr<Scene>		  mScene;

	};
}
