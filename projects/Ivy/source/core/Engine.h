#pragma once

#include "Types.h"
#include "environment/Window.h"
#include "environment/Log.h"
#include "environment/Input.h"
#include "rendering/Renderer.h"

// Forward declare GLFWwindow to avoid including glfw3.h
struct GLFWwindow;

namespace Ivy
{	
	class Engine
	{
	public:
		Engine();
		~Engine();
		
		///
		/// Initializes the engine. Call this befor starting the game loop.
		/// @Param width of window in screen coordinates
		/// @Param height of window in screen coordinates
		///	@Param the title of the window
		///
		void Initialize(int windowWidth, int  windowHeight, const std::string& windowTitle);

		///
		/// Starting the game loop. Call this after the initialization
		///
		void Run();

		///
		/// Use NewFrame() if you want your own game loop. This should be called first in your loop.
		///
		void NewFrame();

		///
		/// Checks if the engine should terminate. 
		/// Triggers if close button of the window is pressed
		/// or something went wrong in the engine
		/// @Return returns true if it should terminate. 
		///
		bool ShouldTerminate();

		// Getter & Setter
		const Ptr<Window> GetWindow() { return mWnd; }

	private:
		Ptr<Window> mWnd;
		Ptr<Renderer> mRenderer;

	};
}
