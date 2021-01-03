#include "BufferGroup.h"

using namespace Euler::Graphics;

void BufferGroup::Create(Vulkan* vulkan, uint32_t count, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	// TODO: assert count, size

	_buffers.resize(count);

	for (int i = 0; i < count; i++)
	{
		vulkan->CreateBuffer(size, usage, properties, _buffers[i].Buffer, _buffers[i].Memory);
	}
}

void BufferGroup::Destroy(Vulkan* vulkan)
{
	for (int i = 0; i < _buffers.size(); i++)
	{
		vulkan->DestroyBuffer(_buffers[i].Buffer, _buffers[i].Memory);
	}
}

Buffer* BufferGroup::Get(int index)
{
	// TODO: assert index
	return &_buffers[index];
}