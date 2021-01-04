#include "DescriptorSetGroup.h"

using namespace Euler::Graphics;

void DescriptorSetGroup::Allocate(Vulkan* vulkan, uint32_t count, VkDescriptorSetLayout layout, VkDescriptorPool pool)
{
	DescriptorSets.resize(count);

	std::vector<VkDescriptorSetLayout> layouts(count, layout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = count;
	allocInfo.pSetLayouts = layouts.data();

	// TODO: Change this with api call
	vkAllocateDescriptorSets(vulkan->_device, &allocInfo, DescriptorSets.data());

	_pool = pool;
}

void DescriptorSetGroup::Free(Vulkan* vulkan)
{
	vkFreeDescriptorSets(vulkan->_device, _pool, DescriptorSets.size(), DescriptorSets.data());
}

void DescriptorSetGroup::UpdateUniformBuffer(Vulkan* vulkan, uint32_t descriptorSetIndex, VkBuffer buffer, uint32_t dstBinding)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = DescriptorSets[descriptorSetIndex];
	write.dstBinding = dstBinding;
	write.dstArrayElement = 0;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write.descriptorCount = 1;
	write.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(vulkan->_device, 1, &write, 0, nullptr);
}

void DescriptorSetGroup::UpdateUniformBufferDynamic(Vulkan* vulkan, uint32_t descriptorSetIndex, VkBuffer buffer, uint32_t dstBinding)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = DescriptorSets[descriptorSetIndex];
	write.dstBinding = dstBinding;
	write.dstArrayElement = 0;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	write.descriptorCount = 1;
	write.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(vulkan->_device, 1, &write, 0, nullptr);
}

void DescriptorSetGroup::UpdateSampler(Vulkan* vulkan, uint32_t descriptorSetIndex, VkImageView imageView, VkSampler sampler, uint32_t dstBinding)
{
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = imageView;
	imageInfo.sampler = sampler;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = DescriptorSets[descriptorSetIndex];
	write.dstBinding = dstBinding;
	write.dstArrayElement = 0;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.descriptorCount = 1;
	write.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(vulkan->_device, 1, &write, 0, nullptr);
}