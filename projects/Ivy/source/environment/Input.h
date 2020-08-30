#pragma once
#include "Window.h"
#include "Types.h"
#include "KeyCodes.h"

namespace Ivy
{
	class Input
	{
	public:
		// Make Engine a friend of this class so it has access to private members
		// because userspace should not get access of Initialize() but it needs to
		// be called in core
		friend class Engine;

		///
		/// Checks if the specified key is being pressed right now
		///
		static bool  IsKeyDown(KeyCode key);

		///
		/// Checks if the specified mouse button is being pressed right now
		///
		static bool  IsMouseButtonDown(MouseCode button);
		
		///
		/// Gets the current mouse position relative to the window
		///
		static Vec2  GetMousePosition();
		
		///
		/// Gets the x coordinate of the current mouse position relative to the window
		///
		static float GetMousePositionX();

		///
		/// Gets the y coordinate of the current mouse position relative to the window
		///
		static float GetMousePositionY();

	private:
		static void Initialize(Ptr<Window> mWnd);

		Input();

		static GLFWwindow* mGlfwWnd;
	};
}