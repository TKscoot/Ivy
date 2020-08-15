#pragma once
//#include "ivypch.h"

#include <string>
#include <memory>

#include "Types.h"

// Forward declare GLFWwindow to avoid including glfw3.h
struct GLFWwindow;

namespace Ivy
{	
	class Window
	{
	public:
		Window(int width, int  height, const std::string& title);
		~Window();

		bool ShouldClose() const noexcept;

		void PollEvents() const noexcept;

		void SwapBuffers() const noexcept;

		VecI2 GetWindowSize() const noexcept;

		GLFWwindow* GetHandle() const noexcept;

		void SetWindowSize(VecI2 size);

		void SetTitle(std::string title);
		
	private:
		static void OnFramebufferSizeChange(GLFWwindow* window, int width, int height);

		GLFWwindow* mWnd;
	};
}
