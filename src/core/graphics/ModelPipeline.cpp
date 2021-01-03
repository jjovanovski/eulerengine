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
	std::vector<VkDescriptorSetLayout> layouts = { ViewProjLayout, ModelLayout, ColorTextureLayout, DirectionalLightLayout };
	pipelineInfo.DescriptorSetLayouts = layouts;

	pipelineInfo.RenderPass = _vulkan->_renderPass;	// TODO: This should be a parameter

	_vulkan->CreatePipeline(&pipelineInfo, &_pipelineLayout, &_pipeline);

	CreateDescriptorSets();
}

void ModelPipeline::Destroy()
{
	_vulkan->DestroyDescriptorPool(_descriptorPool);

	_viewProjBuffers.Destroy(_vulkan);
	_modelBuffers.Destroy(_vulkan);
	_directionalLightBuffers.Destroy(_vulkan);
	_ambientLightBuffers.Destroy(_vulkan);

	_vulkan->DestroyPipeline(_pipelineLayout, _pipeline);

	_vulkan->DestroyDescriptorSetLayout(ViewProjLayout);
	_vulkan->DestroyDescriptorSetLayout(ModelLayout);
	_vulkan->DestroyDescriptorSetLayout(DirectionalLightLayout);
}

std::vector<VertexAttributeInfo> ModelPipeline::GetVertexAttributes()
{
	std::vector<VertexAttributeInfo> vec(3);

	// position
	vec[0].Location = 0;
	vec[0].Offset = 0;
	vec[0].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// normal
	vec[1].Location = 1;
	vec[1].Offset = offsetof(Vertex, Normal);
	vec[1].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// uv
	vec[2].Location = 2;
	vec[2].Offset = offsetof(Vertex, UV);
	vec[2].Format = VK_FORMAT_R32G32_SFLOAT;

	return vec;
}

void ModelPipeline::CreateDescriptorSetLayouts()
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

	/* === ColorTexture DESCRIPTOR SET LAYOUT === */
	std::vector<VkDescriptorSetLayoutBinding> colorTextureBindings(1);
	colorTextureBindings[0].binding = 0;
	colorTextureBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	colorTextureBindings[0].descriptorCount = 1;
	colorTextureBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	_vulkan->CreateDescriptorSetLayout(colorTextureBindings, &ColorTextureLayout);

	/* === DirectionalLight DESCRIPTOR SET LAYOUT === */
	std::vector<VkDescriptorSetLayoutBinding> directionalLightBindings(2);
	directionalLightBindings[0].binding = 0;
	directionalLightBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	directionalLightBindings[0].descriptorCount = 1;
	directionalLightBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	directionalLightBindings[1].binding = 1;
	directionalLightBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	directionalLightBindings[1].descriptorCount = 1;
	directionalLightBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	_vulkan->CreateDescriptorSetLayout(directionalLightBindings, &DirectionalLightLayout);
}

void ModelPipeline::CreateDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === CREATE DESCRIPTOR SET POOL === */

	std::vector<VkDescriptorPoolSize> poolSizes(3);
	poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount };			// ViewProj
	poolSizes[1] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, imageCount };	// Model
	poolSizes[2] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount };			// DirectionalLight

	_vulkan->CreateDescriptorPool(poolSizes, poolSizes.size() * imageCount, &_descriptorPool);

	/* === CREATE DESCRIPTOR SETS === */

	CreateViewProjDescriptorSets();
	CreateModelDescriptorSets();
	CreateDirectionalLightDescriptorSets();
}

void ModelPipeline::CreateViewProjDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();
	
	/* === CREATE ViewProj BUFFERS === */

	_viewProjBuffers.Create(
		_vulkan,
		imageCount,
		sizeof(ViewProj),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	/* === ALLOCATE DESCRIPTOR SETS === */

	std::vector<VkDescriptorSetLayout> layouts(imageCount, ViewProjLayout);
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
		bufferInfo.buffer = _viewProjBuffers.Get(i)->Buffer;
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

	_modelBuffers.Create(
		_vulkan,
		imageCount,
		INITIAL_MODELS_FOR_BUFFER_SIZE * _modelMatrixAlignment,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	/* === ALLOCATE DESCRIPTOR SETS === */

	std::vector<VkDescriptorSetLayout> layouts(imageCount, ModelLayout);
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
		bufferInfo.buffer = _modelBuffers.Get(i)->Buffer;
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

void ModelPipeline::CreateDirectionalLightDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === CREATE DirectioanLight BUFFERS === */

	_directionalLightBuffers.Create(
		_vulkan,
		imageCount,
		sizeof(DirLight),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	_ambientLightBuffers.Create(
		_vulkan,
		imageCount,
		sizeof(AmbLight),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	/* === ALLOCATE DESCRIPTOR SETS === */

	std::vector<VkDescriptorSetLayout> layouts(imageCount, DirectionalLightLayout);
	_directionalLightDescriptorSets.resize(imageCount);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = imageCount;
	allocInfo.pSetLayouts = layouts.data();

	vkAllocateDescriptorSets(_vulkan->_device, &allocInfo, _directionalLightDescriptorSets.data());

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _directionalLightBuffers.Get(i)->Buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorBufferInfo ambientLightBufferInfo{};
		ambientLightBufferInfo.buffer = _ambientLightBuffers.Get(i)->Buffer;
		ambientLightBufferInfo.offset = 0;
		ambientLightBufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writeUbo{};
		writeUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeUbo.dstSet = _directionalLightDescriptorSets[i];
		writeUbo.dstBinding = 0;
		writeUbo.dstArrayElement = 0;
		writeUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeUbo.descriptorCount = 1;
		writeUbo.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet ambientLightWriteUbo{};
		ambientLightWriteUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		ambientLightWriteUbo.dstSet = _directionalLightDescriptorSets[i];
		ambientLightWriteUbo.dstBinding = 1;
		ambientLightWriteUbo.dstArrayElement = 0;
		ambientLightWriteUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		ambientLightWriteUbo.descriptorCount = 1;
		ambientLightWriteUbo.pBufferInfo = &ambientLightBufferInfo;

		VkWriteDescriptorSet writes[] = { writeUbo, ambientLightWriteUbo };

		vkUpdateDescriptorSets(_vulkan->_device, 2, writes, 0, nullptr);
	}
}

void ModelPipeline::RecordCommands(ViewProj viewProjMatrix)
{
	vkCmdBindPipeline(*_vulkan->GetMainCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

	// update viewproj
	void* data;
	vkMapMemory(_vulkan->_device, _viewProjBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(viewProjMatrix), 0, &data);
	memcpy(data, &viewProjMatrix, sizeof(viewProjMatrix));
	vkUnmapMemory(_vulkan->_device, _viewProjBuffers.Get(_vulkan->_currentImage)->Memory);

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

	// update directional light
	void* lightData;
	vkMapMemory(_vulkan->_device, _directionalLightBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(DirectionalLight), 0, &lightData);
	memcpy(lightData, DirLight, sizeof(DirectionalLight));
	vkUnmapMemory(_vulkan->_device, _directionalLightBuffers.Get(_vulkan->_currentImage)->Memory);

	void* ambLightData;
	vkMapMemory(_vulkan->_device, _ambientLightBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(AmbientLight), 0, &ambLightData);
	memcpy(ambLightData, &AmbLight, sizeof(AmbientLight));
	vkUnmapMemory(_vulkan->_device, _ambientLightBuffers.Get(_vulkan->_currentImage)->Memory);

	vkCmdBindDescriptorSets(
		*_vulkan->GetMainCommandBuffer(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_pipelineLayout,
		3,
		1,
		&_directionalLightDescriptorSets[_vulkan->_currentImage],
		0,
		nullptr
	);

	void* modelsData;
	vkMapMemory(_vulkan->_device, _modelBuffers.Get(_vulkan->_currentImage)->Memory, 0, Models.size() * _modelMatrixAlignment, 0, &modelsData);
	for (int i = 0; i < Models.size(); i++)
	{
		Mat4 modelMatrix = Models[i]->GetModelMatrix();
		modelMatrix.Transpose();
		
		size_t dataOffset = _modelMatrixAlignment * i;
		memcpy(dataOffset + static_cast<char*>(modelsData), &modelMatrix, sizeof(modelMatrix));
		memset(dataOffset + static_cast<char*>(modelsData) + sizeof(modelMatrix) + 1, 0, _modelMatrixAlignment - sizeof(modelMatrix));
	}
	vkUnmapMemory(_vulkan->_device, _modelBuffers.Get(_vulkan->_currentImage)->Memory);

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
		for (int j = 0; j < model->Drawables.size(); j++)
		{
			vkCmdBindDescriptorSets(
				*_vulkan->GetMainCommandBuffer(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_pipelineLayout,
				2,
				1,
				&model->Drawables[j]->Texture->DescriptorSets[_vulkan->_currentImage],
				0,
				nullptr
			);

			_vulkan->DrawMesh(
				*_vulkan->GetMainCommandBuffer(),
				&model->Drawables[j]->Mesh->VertexBuffer,
				&model->Drawables[j]->Mesh->IndexBuffer,
				model->Drawables[j]->Mesh->Indices.size()
			);
		}
	}
}