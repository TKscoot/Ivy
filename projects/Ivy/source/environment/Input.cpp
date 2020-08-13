#include "ivypch.h"
#include "Input.h"

GLFWwindow*  Ivy::Input::mGlfwWnd = nullptr;

bool Ivy::Input::IsKeyDown(KeyCode key)
{
	auto state = glfwGetKey(mGlfwWnd, key);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Ivy::Input::IsMouseButtonDown(MouseCode button)
{
	auto state = glfwGetMouseButton(mGlfwWnd, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

Ivy::Vec2 Ivy::Input::GetMousePosition()
{
	double xpos, ypos;
	glfwGetCursorPos(mGlfwWnd, &xpos, &ypos);

	return Vec2(xpos, ypos);
}

float Ivy::Input::GetMousePositionX()
{
	return GetMousePosition().x;
}

float Ivy::Input::GetMousePositionY()
{
	return GetMousePosition().y;
}

void Ivy::Input::Initialize(Ptr<Window> wnd)
{
	mGlfwWnd = wnd->GetHandle();
}
