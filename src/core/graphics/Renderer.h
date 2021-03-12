#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"
#include "RendererInfo.h"

namespace Euler 
{
	namespace Graphics
	{
		class EULER_API Renderer
		{
		protected:
			// TODO: Remove this when we have a better way to reference the global vulkan object
			Vulkan* VulkanRef;
			
			VkPipelineLayout PipelineLayout;
			VkPipeline Pipeline;

			VkRenderPass RenderPass;

			std::vector<DescriptorInfo> DescriptorInfos;

			VkDescriptorPool DescriptorPool;

		public:
			Renderer(Vulkan* vulkan);

			void Create(RendererInfo* rendererInfo);
			void Destroy();
		};
	}
}