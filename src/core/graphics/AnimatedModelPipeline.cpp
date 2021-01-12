#include "AnimatedModelPipeline.h"

#include "../io/Utils.h"
#include "../math/Matrices.h"

using namespace Euler::Graphics;

void AnimatedModelPipeline::Create(Vulkan* vulkan, float viewportWidth, float viewportHeight)
{
	_vulkan = vulkan;

	/* === READ SHADER CODE === */

	std::vector<char> vertexShaderCode = ReadFile("shaders/out/vertex.spv");
	std::vector<char> fragmentShaderCode = ReadFile("shaders/out/fragment.spv");

	/* === CREATE PIPELINE === */

	PipelineInfo pipelineInfo{};

	pipelineInfo.VertexShaderCode = vertexShaderCode.data();
	pipelineInfo.VertexShaderCodeSize = vertexShaderCode.size();

	pipelineInfo.FragmentShaderCode = fragmentShaderCode.data();
	pipelineInfo.FragmentShaderCodeSize = fragmentShaderCode.size();

	pipelineInfo.VertexStride = sizeof(AnimatedVertex);
	pipelineInfo.VertexAttributes = GetVertexAttributes();

	pipelineInfo.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	pipelineInfo.DepthTestEnabled = true;

	pipelineInfo.ViewportWidth = viewportWidth;
	pipelineInfo.ViewportHeight = viewportHeight;

	CreateDescriptorSetLayouts();
	std::vector<VkDescriptorSetLayout> layouts = { ViewProjLayout, ModelLayout, MaterialLayout, DirectionalLightLayout, BoneTransformsLayout };
	pipelineInfo.DescriptorSetLayouts = layouts;

	pipelineInfo.RenderPass = _vulkan->_renderPass;	// TODO: This should be a parameter

	_vulkan->CreatePipeline(&pipelineInfo, &_pipelineLayout, &_pipeline);

	CreateDescriptorSets();
}

void AnimatedModelPipeline::Destroy()
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

std::vector<VertexAttributeInfo> AnimatedModelPipeline::GetVertexAttributes()
{
	std::vector<VertexAttributeInfo> vec(5);

	// position
	vec[0].Location = 0;
	vec[0].Offset = 0;
	vec[0].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// normal
	vec[1].Location = 1;
	vec[1].Offset = offsetof(AnimatedVertex, Normal);
	vec[1].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// uv
	vec[2].Location = 2;
	vec[2].Offset = offsetof(AnimatedVertex, UV);
	vec[2].Format = VK_FORMAT_R32G32_SFLOAT;

	// bone ids
	vec[3].Location = 3;
	vec[3].Offset = offsetof(AnimatedVertex, BoneIds);
	vec[3].Format = VK_FORMAT_R32G32B32A32_SINT;

	// bone weights
	vec[4].Location = 4;
	vec[4].Offset = offsetof(AnimatedVertex, BoneWeights);
	vec[4].Format = VK_FORMAT_R32G32B32_SFLOAT;

	return vec;
}

void AnimatedModelPipeline::CreateDescriptorSetLayouts()
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
	std::vector<VkDescriptorSetLayoutBinding> materialBindings(2);
	materialBindings[0].binding = 0;
	materialBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	materialBindings[0].descriptorCount = 1;
	materialBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	materialBindings[1].binding = 1;
	materialBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	materialBindings[1].descriptorCount = 1;
	materialBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	_vulkan->CreateDescriptorSetLayout(materialBindings, &MaterialLayout);

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

	/* === BoneTransforms DESCRIPTOR SET LAYOUT === */
	std::vector<VkDescriptorSetLayoutBinding> boneTransformBindings(1);
	boneTransformBindings[0].binding = 0;
	boneTransformBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	boneTransformBindings[0].descriptorCount = 1;
	boneTransformBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	_vulkan->CreateDescriptorSetLayout(boneTransformBindings, &BoneTransformsLayout);
}

void AnimatedModelPipeline::CreateDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === CREATE DESCRIPTOR SET POOL === */

	std::vector<VkDescriptorPoolSize> poolSizes(4);
	poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount };					// ViewProj
	poolSizes[1] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, imageCount };			// Model
	poolSizes[2] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount };					// DirectionalLight
	poolSizes[3] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, imageCount };			// BoneTransforms

	_vulkan->CreateDescriptorPool(poolSizes, poolSizes.size() * imageCount, &_descriptorPool);

	/* === CREATE DESCRIPTOR SETS === */

	CreateViewProjDescriptorSets();
	CreateModelDescriptorSets();
	CreateDirectionalLightDescriptorSets();
	CreateBoneTransformDescriptorSets();
}

void AnimatedModelPipeline::CreateViewProjDescriptorSets()
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

	_viewProjDescriptorSetGroup.Allocate(_vulkan, imageCount, ViewProjLayout, _descriptorPool);

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		_viewProjDescriptorSetGroup.UpdateUniformBuffer(_vulkan, i, _viewProjBuffers.Get(i)->Buffer, 0);
	}
}

void AnimatedModelPipeline::CreateModelDescriptorSets()
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

	_modelDescriptorSetGroup.Allocate(_vulkan, imageCount, ModelLayout, _descriptorPool);

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		_modelDescriptorSetGroup.UpdateUniformBufferDynamic(_vulkan, i, _modelBuffers.Get(i)->Buffer, 0);
	}
}

void AnimatedModelPipeline::CreateDirectionalLightDescriptorSets()
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

	_lightDescriptorSetGroup.Allocate(_vulkan, imageCount, DirectionalLightLayout, _descriptorPool);

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		_lightDescriptorSetGroup.UpdateUniformBuffer(_vulkan, i, _directionalLightBuffers.Get(i)->Buffer, 0);
		_lightDescriptorSetGroup.UpdateUniformBuffer(_vulkan, i, _ambientLightBuffers.Get(i)->Buffer, 1);
	}
}

void AnimatedModelPipeline::CreateBoneTransformDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === CALCULATE ALLIGNMENT IN THE UNIFORM BUFFER === */

	uint64_t boneTransformsSize = sizeof(Mat4) * 32;		// TODO: 32 is the maximum number of bones per mesh. Extract constant from this.
	auto minOffset = _vulkan->GetPhysicalDevice()->Properties.limits.minUniformBufferOffsetAlignment;
	_boneTransformMatrixAlignment = (boneTransformsSize + minOffset - 1) & ~(minOffset - 1);

	/* === CREATE Model BUFFERS === */

	_boneTransformBuffers.Create(
		_vulkan,
		imageCount,
		INITIAL_MODELS_FOR_BUFFER_SIZE * boneTransformsSize,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	/* === ALLOCATE DESCRIPTOR SETS === */

	_boneTransformDescriptorSetGroup.Allocate(_vulkan, imageCount, BoneTransformsLayout, _descriptorPool);

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		_boneTransformDescriptorSetGroup.UpdateUniformBufferDynamic(_vulkan, i, _boneTransformBuffers.Get(i)->Buffer, 0);
	}
}

void AnimatedModelPipeline::RecordCommands(ViewProj viewProjMatrix)
{
	vkCmdBindPipeline(*_vulkan->GetMainCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

	// update viewproj
	_vulkan->CopyToMemory(_viewProjBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(viewProjMatrix), &viewProjMatrix);

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

	// update directional light
	_vulkan->CopyToMemory(_directionalLightBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(DirectionalLight), DirLight);

	// update ambient light
	_vulkan->CopyToMemory(_ambientLightBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(AmbientLight), &AmbLight);

	vkCmdBindDescriptorSets(
		*_vulkan->GetMainCommandBuffer(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_pipelineLayout,
		3,
		1,
		&_lightDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
		0,
		nullptr
	);

	void* modelsData;
	_vulkan->MapMemory(_modelBuffers.Get(_vulkan->_currentImage)->Memory, 0, Models.size() * _modelMatrixAlignment, &modelsData);
	for (int i = 0; i < Models.size(); i++)
	{
		Mat4 modelMatrix = Models[i]->Transform.GetModelMatrix();
		modelMatrix.Transpose();

		size_t dataOffset = _modelMatrixAlignment * i;
		memcpy(dataOffset + static_cast<char*>(modelsData), &modelMatrix, sizeof(modelMatrix));
		memset(dataOffset + static_cast<char*>(modelsData) + sizeof(modelMatrix) + 1, 0, _modelMatrixAlignment - sizeof(modelMatrix));
	}
	_vulkan->UnmapMemory(_modelBuffers.Get(_vulkan->_currentImage)->Memory);

	for (int i = 0; i < Models.size(); i++)
	{
		AnimatedModel* model = Models[i];

		// set model matrix
		uint32_t offset = _modelMatrixAlignment * i;
		vkCmdBindDescriptorSets(
			*_vulkan->GetMainCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			1,
			&_modelDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
			1,
			&offset
		);

		// bind bone transforms descriptor set
		void* boneTransformsData;
		_vulkan->MapMemory(_boneTransformBuffers.Get(_vulkan->_currentImage)->Memory, i * _boneTransformMatrixAlignment, _boneTransformMatrixAlignment, &boneTransformsData);
		std::vector<Mat4> matrices(32);
		for (int k = 0; k < 32; k++)
		{
			matrices[k] = Math::Matrices::Identity();
		}
		memcpy(boneTransformsData, matrices.data(), sizeof(Mat4) * 32);
		_vulkan->UnmapMemory(_boneTransformBuffers.Get(_vulkan->_currentImage)->Memory);

		uint32_t boneTransformsOffset = _boneTransformMatrixAlignment * i;
		vkCmdBindDescriptorSets(
			*_vulkan->GetMainCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			4,
			1,
			&_boneTransformDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
			1,
			&boneTransformsOffset
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
				&model->Drawables[j]->Texture->DescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
				0,
				nullptr
			);

			_vulkan->DrawMesh(
				*_vulkan->GetMainCommandBuffer(),
				&model->Drawables[j]->AnimatedMesh->VertexBuffer,
				&model->Drawables[j]->AnimatedMesh->IndexBuffer,
				model->Drawables[j]->AnimatedMesh->Indices.size()
			);
		}
	}
}