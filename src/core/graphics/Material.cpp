#include "Material.h"

using namespace Euler::Graphics;

bool Material::DescriptorPoolCreated = false;
VkDescriptorPool Material::DescriptorPool = VK_NULL_HANDLE;

void Material::Create(Vulkan* vulkan, VkDescriptorSetLayout layout)
{
	_vulkan = vulkan;

	MaterialPropertiesBuffers.Create(
		_vulkan,
		_vulkan->GetSwapchainImageCount(),
		sizeof(MaterialProperties),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	for (int i = 0; i < _vulkan->GetSwapchainImageCount(); i++)
	{
		_vulkan->CopyToMemory(
			MaterialPropertiesBuffers.Get(i)->Memory,
			0,
			sizeof(MaterialProperties),
			&Properties
		);
	}

	if (!Material::DescriptorPoolCreated)
	{
		CreateDescriptorPool();
	}

	MaterialPropertiesDescriptorSetGroup.Allocate(
		_vulkan,
		_vulkan->GetSwapchainImageCount(),
		layout,
		DescriptorPool
	);

	for (int i = 0; i < _vulkan->GetSwapchainImageCount(); i++) 
	{
		MaterialPropertiesDescriptorSetGroup.UpdateUniformBuffer(_vulkan, i, MaterialPropertiesBuffers.Get(i)->Buffer, 0);
	}
}

void Material::Destroy()
{
	MaterialPropertiesDescriptorSetGroup.Free(_vulkan);
	MaterialPropertiesBuffers.Destroy(_vulkan);

	_vulkan->DestroyDescriptorPool(Material::DescriptorPool);
}

void Material::CreateDescriptorPool()
{
	std::vector<VkDescriptorPoolSize> poolSizes(1);
	poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50 };

	_vulkan->CreateDescriptorPool(poolSizes, 50, &Material::DescriptorPool);

	DescriptorPoolCreated = true;
}