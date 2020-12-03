#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "API.h"
#include "graphics/vulkan/Vulkan.h"

namespace Euler
{
	class EULER_API App
	{
	public:
		GLFWwindow* Window;
		Graphics::Vulkan* Vulkan;

		bool WindowMinimized = false;

		App();
	};
}