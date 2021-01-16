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
		static bool  IsKeyBeingPressed(KeyCode key);

		///
		/// Checks if the specified key is being pressed down. Only returns true once the key is being pressed down.
		///
		static bool  IsKeyDown(KeyCode key);

		///
		/// Checks if the specified key is being released
		///
		static bool  IsKeyUp(KeyCode key);

		///
		/// Checks if the specified mouse button is being pressed right now
		///
		static bool  IsMouseButtonDown(MouseCode button);
		
		///
		/// Gets the current mouse position relative to the window
		///
		static Vec2  GetMousePosition();

		///
		/// Gets the mouse position delta of new frame and last frame
		///
		static Vec2  GetMouseDelta();
		
		///
		/// Gets the x coordinate of the current mouse position relative to the window
		///
		static float GetMousePositionX();

		///
		/// Gets the y coordinate of the current mouse position relative to the window
		///
		static float GetMousePositionY();

		///
		/// Hides or shows the mouse cursor
		/// @param should the cursor be hidden (default true)
		///
		static void SetMouseCursorVisible(bool show = true);

		///
		/// Checks if cursor is visible
		/// @return bool true = visible, false = hidden
		///
		static bool GetMouseCursorVisible();

		/*!
		*  
		* Sets the position of the mouse in window space
		* 
		*/
		static void SetMousePositon(Vec2 mousePos);


	private:
		static void Initialize(Ptr<Window> mWnd);

		Input();

		static GLFWwindow* mGlfwWnd;

		static float lastX;
		static float lastY;
		static bool  firstMouse;

		static UnorderedMap<KeyCode, bool> mKeyDown;
	};
}
