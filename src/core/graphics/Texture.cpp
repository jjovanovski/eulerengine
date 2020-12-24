#include "Texture.h"

using namespace Euler::Graphics;

bool Texture::DescriptorPoolCreated = false;
VkDescriptorPool Texture::DescriptorPool = VK_NULL_HANDLE;

void Texture::Create(Vulkan* vulkan, void* pixels, uint32_t width, uint32_t height, size_t size, VkDescriptorSetLayout descriptorSetLayout)
{
	_vulkan = vulkan;

	/* === CREATE STAGING BUFFER === */

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	_vulkan->CreateBuffer(
		size, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, 
		stagingMemory
	);

	void* data;
	vkMapMemory(_vulkan->_device, stagingMemory, 0, size, 0, &data);
	memcpy(data, pixels, size);
	vkUnmapMemory(_vulkan->_device, stagingMemory);

	/* === CREATE IMAGE === */

	_vulkan->CreateImage(
		width,
		height,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		_image,
		_memory
	);

	/* === COPY STAGING BUFFER TO IMAGE */

	_vulkan->TransitionImageLayout(
		_image, 
		VK_FORMAT_R8G8B8A8_SRGB, 
		VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	);

	_vulkan->CopyBufferToImage(stagingBuffer, _image, width, height);

	_vulkan->TransitionImageLayout(
		_image, 
		VK_FORMAT_R8G8B8A8_SRGB, 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);

	_vulkan->DestroyBuffer(stagingBuffer, stagingMemory);

	/* === CREATE IMAGE VIEW === */

	_vulkan->CreateImageView(_image, &_imageView);

	/* === CREATE IMAGE SAMPLER === */

	_vulkan->CreateSampler(&_sampler);

	/* === CREATE DESCRIPTORS === */

	if (!Texture::DescriptorPoolCreated)
	{
		CreateDescriptorPool();
		Texture::DescriptorPoolCreated = true;
	}

	std::vector<VkDescriptorSetLayout> layouts(_vulkan->GetSwapchainImageCount(), descriptorSetLayout);
	DescriptorSets.resize(_vulkan->GetSwapchainImageCount());

	VkDescriptorSetAllocateInfo samplerAllocInfo{};
	samplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	samplerAllocInfo.descriptorPool = Texture::DescriptorPool;
	samplerAllocInfo.descriptorSetCount = _vulkan->GetSwapchainImageCount();
	samplerAllocInfo.pSetLayouts = layouts.data();

	vkAllocateDescriptorSets(_vulkan->_device, &samplerAllocInfo, DescriptorSets.data());

	for (int i = 0; i < DescriptorSets.size(); i++)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = _imageView;
		imageInfo.sampler = _sampler;

		VkWriteDescriptorSet writeSampler{};
		writeSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSampler.dstSet = DescriptorSets[i];
		writeSampler.dstBinding = 0;
		writeSampler.dstArrayElement = 0;
		writeSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeSampler.descriptorCount = 1;
		writeSampler.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(_vulkan->_device, 1, &writeSampler, 0, nullptr);
	}
}

void Texture::Destroy()
{


	if (Texture::DescriptorPoolCreated)
	{
		DestroyDescriptorPool();
	}

	_vulkan->DestroySampler(_sampler);
	_vulkan->DestroyImageView(_imageView);
	_vulkan->DestroyImage(_image, _memory);
}

void Texture::CreateDescriptorPool()
{
	std::vector<VkDescriptorPoolSize> poolSizes(1);
	poolSizes[0] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 50 };

	_vulkan->CreateDescriptorPool(poolSizes, 50, &Texture::DescriptorPool);
}

void Texture::DestroyDescriptorPool()
{
	_vulkan->DestroyDescriptorPool(Texture::DescriptorPool);
}