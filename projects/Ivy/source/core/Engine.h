#pragma once
//#include "ivypch.h"

#include "environment/Window.h"
#include "environment/Log.h"
#include "Types.h"
#include "environment/Input.h"

// Forward declare GLFWwindow to avoid including glfw3.h
struct GLFWwindow;

namespace Ivy
{	
	class Engine
	{
	public:
		Engine();
		~Engine();
		
		void Initialize(int windowWidth, int  windowHeight, const std::string& windowTitle);

		void Run();
		void NewFrame();

		bool ShouldTerminate();

		// Getter & Setter
		const Ptr<Window> GetWindow() { return mWnd; }

	private:
		Ptr<Window> mWnd;

	};
}
