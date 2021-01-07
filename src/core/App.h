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

		void Run();

		virtual void OnStart() {}
		virtual void OnCreate() {}
		virtual void OnUpdate() {}
		virtual void OnDraw() {}
		virtual void OnDestroy() {}
		virtual void OnComplete() {}
	};
}