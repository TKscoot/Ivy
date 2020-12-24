#pragma once
#include "imgui.h"
#include "imgui_themes.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Ivy
{
	/*!
	 * Internal ImGui Class
	 * You don't need to use any of this
	 * 
	 */
	class ImGuiHook
	{
	public:
		ImGuiHook(GLFWwindow* window);
		~ImGuiHook();
	private:
		friend class Scene;


		void NotifyNewFrame();
		void Render();
	};
}