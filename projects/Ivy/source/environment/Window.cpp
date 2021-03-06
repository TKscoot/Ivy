#include "ivypch.h"
#include "Window.h"
#include <GLFW/glfw3.h>

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Ivy
{

	Window::Window(int width, int height, const std::string& title)
	{
		glfwInit();

		// Specifies OpenGL version (4.6) for further use with glfw
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		mMonitor = glfwGetPrimaryMonitor();
		if(mMonitor == nullptr)
		{
			Debug::CoreError("Failed to get primary monitor!");
			return;
		}
		mMode = glfwGetVideoMode(mMonitor);
		if(mMode == nullptr)
		{
			Debug::CoreError("Failed to get monitor video mode!");
			return;
		}
		//mWindowPos = { static_cast<int>(mMode->width / 2), static_cast<int>(mMode->height / 2) };

		mWnd = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		if (mWnd == nullptr)
		{
			Debug::CoreError("Failed to create glfw window!");
			return;
		}

		//glfwSetWindowMonitor(mWnd, mFullscreen ? mMonitor : NULL, 0, 0, mMode->width, mMode->height, GLFW_DONT_CARE);

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

	void Window::PollEvents()
	{
		glfwPollEvents();
		mIsIconified = (bool)glfwGetWindowAttrib(mWnd, GLFW_ICONIFIED);
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
	
	void Window::SetTitle(String title)
	{
		glfwSetWindowTitle(mWnd, title.c_str());
	}

	void Window::SetWindowIcon(String path)
	{
		int x, y, comp;
		GLFWimage icon[1];
		icon[0].pixels = stbi_load(path.c_str(), &x, &y, &comp, STBI_rgb_alpha);
		icon[0].width = x;
		icon[0].height = y;

		glfwSetWindowIcon(mWnd, 1, icon);

		stbi_image_free(icon[0].pixels);
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

	void Window::SetFullscreen(bool fullscreen)
	{
		if(IsFullscreen() == fullscreen)
			return;

		if(fullscreen)
		{
			// backup window position and window size
			glfwGetWindowPos(mWnd, &mWindowPos.x, &mWindowPos.y);
			glfwGetWindowSize(mWnd, &mWindowSize.x, &mWindowSize.y);

			// get resolution of monitor

			mMode = glfwGetVideoMode(mMonitor);

			// switch to full screen
			glfwSetWindowMonitor(mWnd, mMonitor, 0, 0, mMode->width, mMode->height, mMode->refreshRate);

		}
		else
		{
			// restore last window size and position
			glfwSetWindowMonitor(mWnd, nullptr, mWindowPos.x, mWindowPos.y, mWindowSize.x, mWindowSize.y, 0);
		}

		glViewport(0, 0, mWindowSize.x, mWindowSize.y);
	}

	void Window::OnFramebufferSizeChange(GLFWwindow * window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}


}
