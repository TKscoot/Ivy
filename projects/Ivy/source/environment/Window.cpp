#include "ivypch.h"
#include "Window.h"
#include <GLFW/glfw3.h>

namespace Ivy
{
	Window::Window(int width, int height, const std::string& title)
	{
		glfwInit();
		mWnd = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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

	void Window::SetWindowSize(VecI2 size)
	{
		glfwSetWindowSize(mWnd, size.x, size.y);
	}

	void Window::OnFramebufferSizeChange(GLFWwindow * window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}