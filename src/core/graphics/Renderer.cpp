#include "Renderer.h"

using namespace Euler::Graphics;

Renderer::Renderer(Vulkan* vulkan)
{
	VulkanRef = vulkan;
}

void Renderer::Create(RendererInfo* rendererInfo)
{
	RenderPass = rendererInfo->RenderPass;

	// create descriptor layouts
	for (auto& descriptorInfo : DescriptorInfos)
	{
		VulkanRef->CreateDescriptorSetLayout(descriptorInfo.Bindings, &descriptorInfo.Layout);
		
		rendererInfo->DescriptorSetLayouts.push_back(descriptorInfo.Layout);
	}

	// create the pipeline
	VulkanRef->CreatePipeline(rendererInfo, &PipelineLayout, &Pipeline);

	// create pool big enough for the descriptor layouts
	uint32_t imageCount = VulkanRef->GetSwapchainImageCount();
	std::vector<VkDescriptorPoolSize> poolSizes;
	for (auto& descriptorInfo : DescriptorInfos)
	{
		for (auto& binding : descriptorInfo.Bindings)
		{
			VkDescriptorPoolSize poolSize;
			poolSize.type = binding.descriptorType;
			poolSize.descriptorCount = imageCount;
		}
	}
	VulkanRef->CreateDescriptorPool(poolSizes, poolSizes.size() * imageCount, &DescriptorPool);
}

void Renderer::Destroy()
{
	VulkanRef->DestroyPipeline(PipelineLayout, Pipeline);

	for (auto& descriptorInfo : DescriptorInfos)
	{
		VulkanRef->DestroyDescriptorSetLayout(descriptorInfo.Layout);
	}
}
