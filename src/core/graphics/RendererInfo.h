#pragma once

#include "../API.h"
#include "Common.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Euler 
{
	namespace Graphics
	{
		class EULER_API DescriptorInfo 
		{
		public:
			std::vector<VkDescriptorSetLayoutBinding> Bindings;
			VkDescriptorSetLayout Layout;
		};

		class EULER_API RendererInfo
		{
		public:
			const char* VertexShaderCode;
			size_t VertexShaderCodeSize;

			const char* FragmentShaderCode;
			size_t FragmentShaderCodeSize;

			uint32_t VertexStride;
			std::vector<VertexAttributeInfo> VertexAttributes;

			std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;

			VkPrimitiveTopology Topology;

			bool DepthTestEnabled = true;

			float ViewportWidth;
			float ViewportHeight;

			VkRenderPass RenderPass;

			VkCullModeFlags CullModeFlags = VK_CULL_MODE_BACK_BIT;
			VkFrontFace FrontFace = VK_FRONT_FACE_CLOCKWISE;
		};
	}
}