#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "API.h"
#include "vulkan/Instance.h"

namespace Euler
{
	class EULER_API App
	{
	public:
		GLFWwindow* _window;

		App();
	};
}