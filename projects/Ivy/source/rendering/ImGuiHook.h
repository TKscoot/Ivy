#pragma once
#include "environment/Window.h"
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
		/*!
		 * Constructor
		 * 
		 * \param window The glfw program window handle
		 */
		ImGuiHook(Ptr<Ivy::Window> window);
		~ImGuiHook();
		void NotifyNewFrame();
		void Render();
	private:
		friend class Scene;
		friend class Renderer;


	};
}