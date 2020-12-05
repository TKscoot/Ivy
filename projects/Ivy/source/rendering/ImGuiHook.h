#pragma once
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Ivy
{
	class ImGuiHook
	{
	public:
		ImGuiHook(GLFWwindow* window);
		~ImGuiHook();

		void NotifyNewFrame();
		void Render();

	private:
	};
}