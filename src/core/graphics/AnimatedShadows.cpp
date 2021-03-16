#include "AnimatedShadows.h"

#include "../io/Utils.h"
#include "../math/Math.h"

using namespace Euler::Graphics;

void AnimatedShadows::Create(Vulkan* vulkan, AnimatedModelPipeline* modelPipeline, float viewportWidth, float viewportHeight, VkRenderPass renderPass)
{
	_vulkan = vulkan;
	_animatedModelPipeline = modelPipeline;
	_viewportWidth = viewportWidth;
	_viewportHeight = viewportHeight;

	// TODO:
	_viewportWidth = 4096;
	_viewportHeight = 4096;

	/* === READ SHADER CODE === */

	std::vector<char> vertexShaderCode = ReadFile("shaders/out/shadow_animated_vertex.spv");
	std::vector<char> fragmentShaderCode = ReadFile("shaders/out/shadow_animated_fragment.spv");

	/* === CREATE PIPELINE === */

	PipelineInfo pipelineInfo{};

	pipelineInfo.VertexShaderCode = vertexShaderCode.data();
	pipelineInfo.VertexShaderCodeSize = vertexShaderCode.size();

	pipelineInfo.FragmentShaderCode = fragmentShaderCode.data();
	pipelineInfo.FragmentShaderCodeSize = fragmentShaderCode.size();

	pipelineInfo.VertexStride = sizeof(AnimatedVertex);
	pipelineInfo.VertexAttributes = _animatedModelPipeline->GetVertexAttributes();

	pipelineInfo.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	pipelineInfo.DepthTestEnabled = true;

	pipelineInfo.ViewportWidth = _viewportWidth;
	pipelineInfo.ViewportHeight = _viewportHeight;

	CreateDescriptorSetLayouts();
	std::vector<VkDescriptorSetLayout> layouts = { ViewProjLayout, ModelLayout, _animatedModelPipeline->BoneTransformsLayout };
	pipelineInfo.DescriptorSetLayouts = layouts;

	if (renderPass == VK_NULL_HANDLE)
	{
		_vulkan->CreateShadowRenderPass(&_shadowRenderPass);
	}
	else
	{
		_shadowRenderPass = renderPass;
	}
	pipelineInfo.RenderPass = _shadowRenderPass;

	//pipelineInfo.CullMode = VK_CULL_MODE_BACK_BIT;
	//pipelineInfo.FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	_vulkan->CreatePipeline(&pipelineInfo, &_pipelineLayout, &_pipeline);

	CreateFramebuffers();
	UpdateDescriptorSets();
}

void AnimatedShadows::Destroy()
{
	_vulkan->DestroySampler(_sampler);
	_vulkan->DestroyPipeline(_pipelineLayout, _pipeline);
	vkDestroyRenderPass(_vulkan->_device, _shadowRenderPass, nullptr);
}

void AnimatedShadows::CreateFramebuffers()
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

void AnimatedShadows::CreateDescriptorSetLayouts()
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

void AnimatedShadows::UpdateDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === WRITE DESCRIPTOR SETS === */
	
	_viewProjDescriptorSetGroup.Allocate(_vulkan, imageCount, ViewProjLayout, _animatedModelPipeline->_descriptorPool);

	for (int i = 0; i < imageCount; i++)
	{
		_viewProjDescriptorSetGroup.UpdateUniformBuffer(
			_vulkan, 
			i,
			_animatedModelPipeline->_lightViewProjBuffers.Get(i)->Buffer,
			0
		);
	}

	/* === UPDATE SHADOW MAP === */

	_vulkan->CreateSampler(&_sampler);

	for (int i = 0; i < _vulkan->GetSwapchainImageCount(); i++)
	{
		_animatedModelPipeline->_lightDescriptorSetGroup.UpdateSampler(
			_vulkan,
			i,
			_depthImageViews[i],
			_sampler,
			2
		);
	}
}

void AnimatedShadows::RecordCommands(Camera camera)
{
	VkClearValue clearDepth = { 1.0f, 0.0f, 0.0f, 0.0f };

	//VkRenderPassBeginInfo renderPassBeginInfo{};
	//renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	//renderPassBeginInfo.renderPass = _shadowRenderPass;
	//renderPassBeginInfo.framebuffer = _framebuffers[_vulkan->_currentFrame];
	//renderPassBeginInfo.renderArea.extent = { (uint32_t)_viewportWidth, (uint32_t)_viewportHeight };
	//renderPassBeginInfo.renderArea.offset = { 0, 0 };
	//renderPassBeginInfo.clearValueCount = 1;
	//renderPassBeginInfo.pClearValues = &clearDepth;

	//vkCmdBeginRenderPass(*_vulkan->GetMainCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(*_vulkan->GetMainCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

	vkCmdBindDescriptorSets(
		*_vulkan->GetMainCommandBuffer(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_pipelineLayout,
		0,
		1,
		&_viewProjDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
		0,
		nullptr
	);
	
	// render models
	for (int i = 0; i < _animatedModelPipeline->Models.size(); i++)
	{
		AnimatedModel* model = _animatedModelPipeline->Models[i];

		// set model matrix
		uint32_t offset = _animatedModelPipeline->_modelMatrixAlignment * i;
		vkCmdBindDescriptorSets(
			*_vulkan->GetMainCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			1,
			&_animatedModelPipeline->_modelDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
			1,
			&offset
		);

		// draw model meshes
		for (int j = 0; j < model->Drawables.size(); j++)
		{
			_vulkan->DrawMesh(
				*_vulkan->GetMainCommandBuffer(),
				&model->Drawables[j]->AnimatedMesh->VertexBuffer,
				&model->Drawables[j]->AnimatedMesh->IndexBuffer,
				model->Drawables[j]->AnimatedMesh->Indices.size()
			);
		}
	}

	vkCmdEndRenderPass(*_vulkan->GetMainCommandBuffer());
}