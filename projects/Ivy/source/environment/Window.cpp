#include "ivypch.h"
#include "Window.h"
#include <GLFW/glfw3.h>

namespace Ivy
{
	Window::Window(int width, int height, const std::string& title)
	{
		glfwInit();

		// Specifies OpenGL version (4.6) for further use with glfw
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		mWnd = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		if (mWnd == nullptr)
		{
			Debug::CoreError("Failed to create glfw window!");
			return;
		}

		glfwMakeContextCurrent(mWnd);

		glfwSetFramebufferSizeCallback(mWnd, &Window::OnFramebufferSizeChange);
	}

	Window::~Window()
	{
		glfwDestroyWindow(mWnd);
		glfwTerminate();
		mWnd = nullptr;
	}

	bool Window::ShouldClose() const noexcept
	{
		return glfwWindowShouldClose(mWnd) != 0;
	}

	void Window::PollEvents() const noexcept
	{
		glfwPollEvents();
	}

	void Window::SwapBuffers() const noexcept
	{
		glfwSwapBuffers(mWnd);
	}

	VecI2 Window::GetWindowSize() const noexcept
	{
		VecI2 sz;
		glfwGetWindowSize(mWnd, &sz.x, &sz.y);
		return sz;
	}

	GLFWwindow * Window::GetHandle() const noexcept
	{
		return mWnd;
	}
	
	void Window::SetTitle(std::string title)
	{
		glfwSetWindowTitle(mWnd, title.c_str());
	}

	void Window::EnableVsync(bool enable)
	{
		int interval = (int)enable;
		glfwSwapInterval(interval);
	}

	void Window::Finalize()
	{
		glfwTerminate();
	}

	void Window::SetWindowSize(VecI2 size)
	{
		glfwSetWindowSize(mWnd, size.x, size.y);
	}

	void Window::OnFramebufferSizeChange(GLFWwindow * window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}