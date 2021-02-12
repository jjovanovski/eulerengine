#include "Shadows.h"

#include "../io/Utils.h"

using namespace Euler::Graphics;

void Shadows::Create(Vulkan* vulkan, ModelPipeline* modelPipeline, float viewportWidth, float viewportHeight)
{
	_vulkan = vulkan;
	_modelPipeline = modelPipeline;
	_viewportWidth = viewportWidth;
	_viewportHeight = viewportHeight;

	/* === READ SHADER CODE === */

	std::vector<char> vertexShaderCode = ReadFile("shaders/out/shadow_vertex.spv");
	std::vector<char> fragmentShaderCode = ReadFile("shaders/out/shadow_fragment.spv");

	/* === CREATE PIPELINE === */

	PipelineInfo pipelineInfo{};

	pipelineInfo.VertexShaderCode = vertexShaderCode.data();
	pipelineInfo.VertexShaderCodeSize = vertexShaderCode.size();

	pipelineInfo.FragmentShaderCode = fragmentShaderCode.data();
	pipelineInfo.FragmentShaderCodeSize = fragmentShaderCode.size();

	pipelineInfo.VertexStride = sizeof(Vertex);
	pipelineInfo.VertexAttributes = modelPipeline->GetVertexAttributes();

	pipelineInfo.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	pipelineInfo.DepthTestEnabled = true;

	pipelineInfo.ViewportWidth = viewportWidth;
	pipelineInfo.ViewportHeight = viewportHeight;

	CreateDescriptorSetLayouts();
	std::vector<VkDescriptorSetLayout> layouts = { ViewProjLayout, ModelLayout };
	pipelineInfo.DescriptorSetLayouts = layouts;

	_vulkan->CreateShadowRenderPass(&_shadowRenderPass);
	pipelineInfo.RenderPass = _shadowRenderPass;

	_vulkan->CreatePipeline(&pipelineInfo, &_pipelineLayout, &_pipeline);

	CreateFramebuffers();
}

void Shadows::Destroy()
{
	_vulkan->DestroyPipeline(_pipelineLayout, _pipeline);
	vkDestroyRenderPass(_vulkan->_device, _shadowRenderPass, nullptr);
}

void Shadows::CreateFramebuffers()
{
	/* === CREATE IMAGES AND IMAGE VIEWS === */

	_depthImages.resize(_vulkan->GetSwapchainImageCount());
	_depthImageMemories.resize(_vulkan->GetSwapchainImageCount());
	_depthImageViews.resize(_vulkan->GetSwapchainImageCount());

	for (int i = 0; i < _vulkan->GetSwapchainImageCount(); i++)
	{
		_vulkan->CreateImage(
			(uint32_t)_viewportWidth,
			(uint32_t)_viewportHeight,
			VK_FORMAT_D32_SFLOAT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			_depthImages[i],
			_depthImageMemories[i]
		);

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.format = VK_FORMAT_D32_SFLOAT;
		imageViewCreateInfo.image = _depthImages[i];
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		vkCreateImageView(_vulkan->_device, &imageViewCreateInfo, nullptr, &_depthImageViews[i]);
	}

	/* === CREATE FRAMEBUFFERS === */

	_framebuffers.resize(_vulkan->GetSwapchainImageCount());

	for (int i = 0; i < _vulkan->GetSwapchainImageCount(); i++)
	{
		VkImageView attachments[] = { _depthImageViews[i] };

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.width = (uint32_t)_viewportWidth;
		createInfo.height = (uint32_t)_viewportHeight;
		createInfo.layers = 1;
		createInfo.renderPass = _shadowRenderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = attachments;

		vkCreateFramebuffer(_vulkan->_device, &createInfo, nullptr, &_framebuffers[i]);
	}
}

void Shadows::CreateDescriptorSetLayouts()
{
	/* === ViewProj DESCRIPTOR SET LAYOUT === */
	std::vector<VkDescriptorSetLayoutBinding> viewProjBindings(1);
	viewProjBindings[0].binding = 0;
	viewProjBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	viewProjBindings[0].descriptorCount = 1;
	viewProjBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	_vulkan->CreateDescriptorSetLayout(viewProjBindings, &ViewProjLayout);

	/* === Model DESCRIPTOR SET LAYOUT === */
	std::vector<VkDescriptorSetLayoutBinding> modelBindings(1);
	modelBindings[0].binding = 0;
	modelBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	modelBindings[0].descriptorCount = 1;
	modelBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	_vulkan->CreateDescriptorSetLayout(modelBindings, &ModelLayout);
}

void Shadows::RecordCommands()
{
	VkClearValue clearDepth = { 1.0f, 0.0f, 0.0f, 0.0f };

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = _shadowRenderPass;
	renderPassBeginInfo.framebuffer = _framebuffers[_vulkan->_currentFrame];
	renderPassBeginInfo.renderArea.extent = { (uint32_t)_viewportWidth, (uint32_t)_viewportHeight };
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearDepth;

	vkCmdBeginRenderPass(*_vulkan->GetMainCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(*_vulkan->GetMainCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

	vkCmdBindDescriptorSets(
		*_vulkan->GetMainCommandBuffer(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_pipelineLayout,
		0,
		1,
		&_modelPipeline->_viewProjDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
		0,
		nullptr
	);

	for (int i = 0; i < _modelPipeline->Models.size(); i++)
	{
		Model* model = _modelPipeline->Models[i];

		// set model matrix
		uint32_t offset = _modelPipeline->_modelMatrixAlignment * i;
		vkCmdBindDescriptorSets(
			*_vulkan->GetMainCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			1,
			&_modelPipeline->_modelDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
			1,
			&offset
		);

		// draw model meshes
		for (int j = 0; j < model->Drawables.size(); j++)
		{
			_vulkan->DrawMesh(
				*_vulkan->GetMainCommandBuffer(),
				&model->Drawables[j]->Mesh->VertexBuffer,
				&model->Drawables[j]->Mesh->IndexBuffer,
				model->Drawables[j]->Mesh->Indices.size()
			);
		}
	}


	vkCmdEndRenderPass(*_vulkan->GetMainCommandBuffer());
}