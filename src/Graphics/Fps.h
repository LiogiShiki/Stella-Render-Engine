#pragma once

#include<glfw3.h>
// For glfwGetTime()

#include <sstream>

namespace Lumen
{
	void DisplayFrameRate(GLFWwindow* pWindow, const std::string& title);
}