#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"

#include <stdint.h>

namespace Euler
{
	namespace Graphics
	{
		class EULER_API Texture
		{
		private:
			Vulkan* _vulkan;

			VkImage _image;
			VkDeviceMemory _memory;
			VkImageView _imageView;
			VkSampler _sampler;

			static bool DescriptorPoolCreated;
			static VkDescriptorPool DescriptorPool;

		public:
			std::vector<VkDescriptorSet> DescriptorSets;

			void Create(Vulkan* vulkan, void* pixels, uint32_t width, uint32_t height, size_t size, VkDescriptorSetLayout descriptorSetLayout);
			void Destroy();

		private:
			void CreateDescriptorPool();
			void DestroyDescriptorPool();
		};
	}
}