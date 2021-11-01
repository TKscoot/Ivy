#pragma once

#include "Types.h"
#include "environment/Window.h"
#include "environment/Log.h"
#include "environment/Input.h"
#include "rendering/Renderer.h"
#include "scene/Entity.h"
#include "scene/SceneManager.h"
#include "core/Timer.h"

// Forward declare GLFWwindow to avoid including glfw3.h
struct GLFWwindow;

namespace Ivy
{	
	class Engine
	{
	public:

		/*!
		 *  Default constructor
		 * 
		 */
		Engine();
		~Engine();
		

		/*!
		 * Initializes the engine. Call this befor starting the game loop.
		 * 
		 * \param windowWidth width of window in screen coordinates
		 * \param windowHeight height of window in screen coordinates
		 * \param windowTitle the title of the window
		 */
		void Initialize(int windowWidth, int  windowHeight, const std::string& windowTitle);

		/*!
		 * Starting the game loop. Call this after the initialization
		 * 
		 */
		void Run();

		/*!
		 * Use NewFrame() if you want your own game loop. This should be called first in your loop.
		 * 
		 */
		void NewFrame();

		/*!
		 * Checks if the engine should terminate. 
		 * Triggers if close button of the window is pressed
		 * or something went wrong in the engine
		 * 
		 * \return returns true if it should terminate. 
		 */
		bool ShouldTerminate();

		// Getter & Setter

		/*!
		 * Gets the window instance
		 * 
		 * \return const pointer to window
		 */
		const Ptr<Window> GetWindow() { return mWnd; }

		/*!
		 * Gets delta time in ms
		 * 
		 * \return const float delta time
		 */
		const float GetDeltaTime() { return mDeltaTime; }

		/*!
		 * Tells Ivy to shutdown
		 * 
		 */
		static void Terminate() { mTerminate = true; }

		const Ptr<Renderer> GetRenderer() const { return mRenderer; }

	private:
		void CheckGLVersion(int supportedMajor, int supportedMinor);

		Ptr<Window>   mWnd		    = nullptr;
		Ptr<Renderer> mRenderer		= nullptr;

		Ptr<Scene>    mCurrentScene = nullptr;
		Vector<Ptr<Scene>> mScenes;

		float mDeltaTime = 0;
		static inline bool mTerminate = false;

	};
}
