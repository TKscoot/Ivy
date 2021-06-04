#include "ivypch.h"
#include "ImGuiHook.h"

Ivy::ImGuiHook::ImGuiHook(Ptr<Window> window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	// Setup Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window->GetHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 450 core");

	// Setup Dear ImGui style
	ImGui::StyleDarkAndPurple();
}

Ivy::ImGuiHook::~ImGuiHook()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Ivy::ImGuiHook::NotifyNewFrame()
{
	// Tell ImGui new frame has started
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Ivy::ImGuiHook::Render()
{
	// Render ImGui stuff
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
