#include "ivypch.h"
#include "Input.h"

GLFWwindow*  Ivy::Input::mGlfwWnd	= nullptr;
float		 Ivy::Input::lastX		= 0;
float		 Ivy::Input::lastY	    = 0;
bool		 Ivy::Input::firstMouse = true;

bool Ivy::Input::IsKeyDown(KeyCode key)
{
	auto state = glfwGetKey(mGlfwWnd, key);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Ivy::Input::IsKeyUp(KeyCode key)
{
	auto state = glfwGetKey(mGlfwWnd, key);
	return state == GLFW_RELEASE;
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

Ivy::Vec2 Ivy::Input::GetMouseDelta()
{
	Vec2 mousePos = GetMousePosition();

	if (firstMouse)
	{
		lastX = mousePos.x;
		lastY = mousePos.y;
		firstMouse = false;
	}

	Vec2 delta = Vec2(0.0f);
	delta.x = mousePos.x - lastX;
	delta.y = lastY - mousePos.y;

	lastX = mousePos.x;
	lastY = mousePos.y;

	return delta;
}

float Ivy::Input::GetMousePositionX()
{
	return GetMousePosition().x;
}

float Ivy::Input::GetMousePositionY()
{
	return GetMousePosition().y;
}

void Ivy::Input::SetMouseCursorVisible(bool show)
{
	if (!show)
	{
		glfwSetInputMode(mGlfwWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetInputMode(mGlfwWnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

bool Ivy::Input::GetMouseCursorVisible()
{
	auto cursor = glfwGetInputMode(mGlfwWnd, GLFW_CURSOR);
	if(cursor == GLFW_CURSOR_DISABLED)
	{
		return false;
	}
	else if(cursor == GLFW_CURSOR_NORMAL)
	{
		return true;
	}

	return false;
}

void Ivy::Input::Initialize(Ptr<Window> wnd)
{
	mGlfwWnd = wnd->GetHandle();
	lastX = wnd->GetWindowSize().x;
	lastY = wnd->GetWindowSize().y;
}
