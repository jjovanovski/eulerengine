#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"

#include <stdint.h>

namespace Euler
{
	namespace Graphics
	{
		class EULER_API BufferGroup
		{
		private:
			std::vector<Buffer> _buffers;

		public:
			void Create(Vulkan* vulkan, uint32_t count, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
			void Destroy(Vulkan* vulkan);

			Buffer* Get(int index);
		};
	}
}