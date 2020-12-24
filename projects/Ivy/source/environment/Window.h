#pragma once
//#include "ivypch.h"

#include <string>
#include <memory>

#include "Types.h"
#include "environment/Log.h"

// Forward declare GLFWwindow to avoid including glfw3.h
struct GLFWwindow;

namespace Ivy
{	
	class Window
	{
	public:
		/*!
		 * Window constructor
		 * 
		 * \param width width of the window
		 * \param height height of the window
		 * \param title title of the window
		 */
		Window(int width, int  height, const std::string& title);
		~Window();

		///
		/// Notifies if the windows should be closed
		/// @return returns true if window will close
		///
		bool ShouldClose() const noexcept;

		///
		/// Polls window and input events and updates window
		///
		void PollEvents() const noexcept;

		///
		/// Swaps the pixelbuffers of the window
		///
		void SwapBuffers() const noexcept;

		///
		/// Gets the size of the window in screen coordinates
		/// @return returns vector with x as width and y as height
		///
		VecI2 GetWindowSize() const noexcept;

		///
		/// Gets the GLFW window handle
		/// @return Returns pointer to GLFWwindow
		///
		GLFWwindow* GetHandle() const noexcept;

		///
		/// Sets the size of the window
		/// @param size Vector for size of the window. x = height, y = width
		///
		void SetWindowSize(VecI2 size);

		///
		/// Sets the title of the window
		/// @param title window title string
		///
		void SetTitle(std::string title);

		///
		/// Sets the icon of the window
		/// @param path The path to the icon
		///
		void SetWindowIcon(std::string path);

		///
		/// Enables/Disables vsync
		/// @param enable wether to enable vsync
		///
		void EnableVsync(bool enable);

		///
		/// Cleans up all window resources
		///
		void Finalize();
		
	private:
		static void OnFramebufferSizeChange(GLFWwindow* window, int width, int height);

		GLFWwindow* mWnd;
	};
}
