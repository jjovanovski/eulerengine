#include "ModelPipeline.h"

#include "../io/Utils.h"
#include "../math/Matrices.h"

using namespace Euler::Graphics;

void ModelPipeline::Create(Vulkan* vulkan, float viewportWidth, float viewportHeight)
{
	_vulkan = vulkan;

	/* === READ SHADER CODE === */

	std::vector<char> vertexShaderCode = ReadFile("shaders/vertex.spv");
	std::vector<char> fragmentShaderCode = ReadFile("shaders/fragment.spv");

	/* === CREATE PIPELINE === */

	PipelineInfo pipelineInfo{};
	
	pipelineInfo.VertexShaderCode = vertexShaderCode.data();
	pipelineInfo.VertexShaderCodeSize = vertexShaderCode.size();

	pipelineInfo.FragmentShaderCode = fragmentShaderCode.data();
	pipelineInfo.FragmentShaderCodeSize = fragmentShaderCode.size();

	pipelineInfo.VertexStride = sizeof(Vertex);
	pipelineInfo.VertexAttributes = GetVertexAttributes();

	pipelineInfo.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	pipelineInfo.DepthTestEnabled = true;

	pipelineInfo.ViewportWidth = viewportWidth;
	pipelineInfo.ViewportHeight = viewportHeight;

	CreateDescriptorSetLayouts();
	std::vector<VkDescriptorSetLayout> layouts = { _viewProjLayout, _modelLayout };
	pipelineInfo.DescriptorSetLayouts = layouts;

	pipelineInfo.RenderPass = _vulkan->_renderPass;	// TODO: This should be a parameter

	_vulkan->CreatePipeline(&pipelineInfo, &_pipelineLayout, &_pipeline);

	CreateDescriptorSets();
}

void ModelPipeline::Destroy()
{
	vkDestroyDescriptorPool(_vulkan->_device, _descriptorPool, nullptr);

	uint32_t vpc = _viewProjBuffers.size();
	uint32_t mc = _modelBuffers.size();
	for (int i = 0; i < _viewProjBuffers.size(); i++)
	{
		_vulkan->DestroyBuffer(_viewProjBuffers[i].Buffer, _viewProjBuffers[i].Memory);
	}
	for (int i = 0; i < _modelBuffers.size(); i++)
	{
		_vulkan->DestroyBuffer(_modelBuffers[i].Buffer, _modelBuffers[i].Memory);
	}

	_vulkan->DestroyPipeline(_pipelineLayout, _pipeline);

	_vulkan->DestroyDescriptorSetLayout(_viewProjLayout);
	_vulkan->DestroyDescriptorSetLayout(_modelLayout);
}

std::vector<VertexAttributeInfo> ModelPipeline::GetVertexAttributes()
{
	std::vector<VertexAttributeInfo> vec(3);

	// position
	vec[0].Location = 0;
	vec[0].Offset = 0;
	vec[0].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// color
	vec[1].Location = 1;
	vec[1].Offset = offsetof(Vertex, Color);
	vec[1].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// uv
	vec[2].Location = 2;
	vec[2].Offset = offsetof(Vertex, Uv);
	vec[2].Format = VK_FORMAT_R32G32_SFLOAT;

	return vec;
}

void ModelPipeline::CreateDescriptorSetLayouts()
{
	/* === ViewProj DESCRIPTOR SET LAYOUT */
	std::vector<VkDescriptorSetLayoutBinding> viewProjBindings(1);
	viewProjBindings[0].binding = 0;
	viewProjBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	viewProjBindings[0].descriptorCount = 1;
	viewProjBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	_vulkan->CreateDescriptorSetLayout(viewProjBindings, &_viewProjLayout);

	/* === Model DESCRIPTOR SET LAYOUT */
	std::vector<VkDescriptorSetLayoutBinding> modelBindings(1);
	modelBindings[0].binding = 0;
	modelBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	modelBindings[0].descriptorCount = 1;
	modelBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	_vulkan->CreateDescriptorSetLayout(modelBindings, &_modelLayout);
}

void ModelPipeline::CreateDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === CREATE DESCRIPTOR SET POOL === */

	std::vector<VkDescriptorPoolSize> poolSizes(2);
	poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount };			// ViewProj
	poolSizes[1] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, imageCount };	// Model

	_vulkan->CreateDescriptorPool(poolSizes, poolSizes.size() * imageCount, &_descriptorPool);

	/* === CREATE DESCRIPTOR SETS === */

	CreateViewProjDescriptorSets();
	CreateModelDescriptorSets();
}

void ModelPipeline::CreateViewProjDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();
	
	/* === CREATE ViewProj BUFFERS === */

	_viewProjBuffers.resize(imageCount);
	for (int i = 0; i < imageCount; i++)
	{
		_vulkan->CreateBuffer(
			sizeof(ViewProj),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			_viewProjBuffers[i].Buffer,
			_viewProjBuffers[i].Memory
		);
	}

	/* === ALLOCATE DESCRIPTOR SETS === */

	std::vector<VkDescriptorSetLayout> layouts(imageCount, _viewProjLayout);
	_viewProjDescriptorSets.resize(imageCount);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = imageCount;
	allocInfo.pSetLayouts = layouts.data();

	vkAllocateDescriptorSets(_vulkan->_device, &allocInfo, _viewProjDescriptorSets.data());

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _viewProjBuffers[i].Buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writeUbo{};
		writeUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeUbo.dstSet = _viewProjDescriptorSets[i];
		writeUbo.dstBinding = 0;
		writeUbo.dstArrayElement = 0;
		writeUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeUbo.descriptorCount = 1;
		writeUbo.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(_vulkan->_device, 1, &writeUbo, 0, nullptr);
	}
}

void ModelPipeline::CreateModelDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === CALCULATE THE MODEL MATRIX ALLIGNMENT IN THE UNIFORM BUFFER === */

	auto minOffset = _vulkan->GetPhysicalDevice()->Properties.limits.minUniformBufferOffsetAlignment;
	_modelMatrixAlignment = (sizeof(Mat4) + minOffset - 1) & ~(minOffset - 1);

	/* === CREATE Model BUFFERS === */

	_modelBuffers.resize(imageCount);
	for (int i = 0; i < imageCount; i++)
	{
		_vulkan->CreateBuffer(
			INITIAL_MODELS_FOR_BUFFER_SIZE * sizeof(Mat4),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			_modelBuffers[i].Buffer,
			_modelBuffers[i].Memory
		);
	}

	/* === ALLOCATE DESCRIPTOR SETS === */

	std::vector<VkDescriptorSetLayout> layouts(imageCount, _modelLayout);
	_modelDescriptorSets.resize(imageCount);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = imageCount;
	allocInfo.pSetLayouts = layouts.data();

	vkAllocateDescriptorSets(_vulkan->_device, &allocInfo, _modelDescriptorSets.data());

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _modelBuffers[i].Buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writeUbo{};
		writeUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeUbo.dstSet = _modelDescriptorSets[i];
		writeUbo.dstBinding = 0;
		writeUbo.dstArrayElement = 0;
		writeUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		writeUbo.descriptorCount = 1;
		writeUbo.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(_vulkan->_device, 1, &writeUbo, 0, nullptr);
	}
}

void ModelPipeline::RecordCommands()
{
	vkCmdBindPipeline(*_vulkan->GetMainCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

	// update viewproj
	ViewProj mvp;
	mvp.View = Math::Matrices::Translate(0, 0, 3);
	mvp.Projection = Euler::Math::Matrices::Perspective(1200, 800, 60, 0.001f, 100.0f);
	mvp.View.Transpose();
	mvp.Projection.Transpose();

	void* data;
	vkMapMemory(_vulkan->_device, _viewProjBuffers[_vulkan->_currentImage].Memory, 0, sizeof(mvp), 0, &data);
	memcpy(data, &mvp, sizeof(mvp));
	vkUnmapMemory(_vulkan->_device, _viewProjBuffers[_vulkan->_currentImage].Memory);

	vkCmdBindDescriptorSets(
		*_vulkan->GetMainCommandBuffer(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_pipelineLayout,
		0,
		1,
		&_viewProjDescriptorSets[_vulkan->_currentImage],
		0,
		nullptr
	);

	void* modelsData;
	vkMapMemory(_vulkan->_device, _modelBuffers[_vulkan->_currentImage].Memory, 0, Models.size() * _modelMatrixAlignment, 0, &modelsData);
	for (int i = 0; i < Models.size(); i++)
	{
		Mat4 modelMatrix = Models[i]->GetModelMatrix();
		modelMatrix.Transpose();
		
		size_t dataOffset = _modelMatrixAlignment * i;
		memcpy(dataOffset + static_cast<char*>(modelsData), &modelMatrix, sizeof(modelMatrix));
		memset(dataOffset + static_cast<char*>(modelsData) + sizeof(modelMatrix) + 1, 0, _modelMatrixAlignment - sizeof(modelMatrix));
	}
	vkUnmapMemory(_vulkan->_device, _modelBuffers[_vulkan->_currentImage].Memory);

	for (int i = 0; i < Models.size(); i++)
	{
		Model* model = Models[i];
		
		// set model matrix
		uint32_t offset = _modelMatrixAlignment * i;
		vkCmdBindDescriptorSets(
			*_vulkan->GetMainCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			1,
			&_modelDescriptorSets[_vulkan->_currentImage],
			1,
			&offset
		);

		// draw model meshes
		for (int j = 0; j < model->Meshes.size(); j++)
		{
			_vulkan->DrawMesh(
				*_vulkan->GetMainCommandBuffer(),
				&model->Meshes[j]->VertexBuffer,
				&model->Meshes[j]->IndexBuffer,
				model->Meshes[j]->Indices.size()
			);
		}
	}
}