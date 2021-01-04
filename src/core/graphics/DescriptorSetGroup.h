#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"

#include <vector>
#include <stdint.h>

namespace Euler
{
	namespace Graphics
	{
		class EULER_API DescriptorSetGroup
		{
		private:
			VkDescriptorPool _pool;

		public:
			std::vector<VkDescriptorSet> DescriptorSets;

			void Allocate(Vulkan* vulkan, uint32_t count, VkDescriptorSetLayout layout, VkDescriptorPool pool);
			void Free(Vulkan* vulkan);

			void UpdateUniformBuffer(Vulkan* vulkan, uint32_t descriptorSetIndex, VkBuffer buffer, uint32_t dstBinding);
			void UpdateUniformBufferDynamic(Vulkan* vulkan, uint32_t descriptorSetIndex, VkBuffer buffer, uint32_t dstBinding);
			void UpdateSampler(Vulkan* vulkan, uint32_t descriptorSetIndex, VkImageView imageView, VkSampler sampler, uint32_t dstBinding);
		};
	}
};