#include "ModelPipeline.h"

#include "../io/Utils.h"
#include "../math/Matrices.h"

using namespace Euler::Graphics;

void ModelPipeline::Create(Vulkan* vulkan, float viewportWidth, float viewportHeight)
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

	pipelineInfo.VertexStride = sizeof(Vertex);
	pipelineInfo.VertexAttributes = GetVertexAttributes();

	pipelineInfo.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	pipelineInfo.DepthTestEnabled = true;

	pipelineInfo.ViewportWidth = viewportWidth;
	pipelineInfo.ViewportHeight = viewportHeight;

	CreateDescriptorSetLayouts();
	std::vector<VkDescriptorSetLayout> layouts = { ViewProjLayout, ModelLayout, MaterialLayout, DirectionalLightLayout, LightViewProjLayout, NormalMapLayout, MaterialPropertiesLayout };
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
	std::vector<VertexAttributeInfo> vec(5);

	// position
	vec[0].Location = 0;
	vec[0].Offset = 0;
	vec[0].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// normal
	vec[1].Location = 1;
	vec[1].Offset = offsetof(Vertex, Normal);
	vec[1].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// tangent
	vec[2].Location = 2;
	vec[2].Offset = offsetof(Vertex, Tangent);
	vec[2].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// bitangent
	vec[3].Location = 3;
	vec[3].Offset = offsetof(Vertex, Bitangent);
	vec[3].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// uv
	vec[4].Location = 4;
	vec[4].Offset = offsetof(Vertex, UV);
	vec[4].Format = VK_FORMAT_R32G32_SFLOAT;

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
	std::vector<VkDescriptorSetLayoutBinding> materialBindings(1);
	materialBindings[0].binding = 0;
	materialBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	materialBindings[0].descriptorCount = 1;
	materialBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	_vulkan->CreateDescriptorSetLayout(materialBindings, &MaterialLayout);

	/* === NormalMap DESCRIPTOR SET LAYOUT === */
	std::vector<VkDescriptorSetLayoutBinding> normalMapBindings(1);

	normalMapBindings[0].binding = 0;
	normalMapBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalMapBindings[0].descriptorCount = 1;
	normalMapBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	_vulkan->CreateDescriptorSetLayout(normalMapBindings, &NormalMapLayout);

	/* === MaterialProperties DESCRIPTOR SET LAYOUT */
	std::vector<VkDescriptorSetLayoutBinding> materialProperties(1);

	materialProperties[0].binding = 0;
	materialProperties[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	materialProperties[0].descriptorCount = 1;
	materialProperties[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	_vulkan->CreateDescriptorSetLayout(materialProperties, &MaterialPropertiesLayout);

	/* === DirectionalLight DESCRIPTOR SET LAYOUT === */
	std::vector<VkDescriptorSetLayoutBinding> directionalLightBindings(3);
	directionalLightBindings[0].binding = 0;
	directionalLightBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	directionalLightBindings[0].descriptorCount = 1;
	directionalLightBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	directionalLightBindings[1].binding = 1;
	directionalLightBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	directionalLightBindings[1].descriptorCount = 1;
	directionalLightBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	directionalLightBindings[2].binding = 2;
	directionalLightBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	directionalLightBindings[2].descriptorCount = 1;
	directionalLightBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	_vulkan->CreateDescriptorSetLayout(directionalLightBindings, &DirectionalLightLayout);

	/* === LightViewProj DESCRIPTOR SET LAYOUT === */

	std::vector<VkDescriptorSetLayoutBinding> lightViewProjBindings(1);
	lightViewProjBindings[0].binding = 0;
	lightViewProjBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	lightViewProjBindings[0].descriptorCount = 1;
	lightViewProjBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	_vulkan->CreateDescriptorSetLayout(lightViewProjBindings, &LightViewProjLayout);
}

void ModelPipeline::CreateDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === CREATE DESCRIPTOR SET POOL === */

	std::vector<VkDescriptorPoolSize> poolSizes(5);
	poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount*2 };			// ViewProj
	poolSizes[3] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, imageCount };	// Model
	poolSizes[2] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount };			// DirectionalLight
	poolSizes[3] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageCount };	// shadows
	poolSizes[4] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount };			// LightViewProj

	_vulkan->CreateDescriptorPool(poolSizes, poolSizes.size() * imageCount, &_descriptorPool);

	/* === CREATE DESCRIPTOR SETS === */

	CreateViewProjDescriptorSets();
	CreateModelDescriptorSets();
	CreateDirectionalLightDescriptorSets();
	CreateLightViewProjDescriptorSets();
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

	_viewProjDescriptorSetGroup.Allocate(_vulkan, imageCount, ViewProjLayout, _descriptorPool);

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		_viewProjDescriptorSetGroup.UpdateUniformBuffer(_vulkan, i, _viewProjBuffers.Get(i)->Buffer, 0);
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

	_modelDescriptorSetGroup.Allocate(_vulkan, imageCount, ModelLayout, _descriptorPool);

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		_modelDescriptorSetGroup.UpdateUniformBufferDynamic(_vulkan, i, _modelBuffers.Get(i)->Buffer, 0);
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

	_lightDescriptorSetGroup.Allocate(_vulkan, imageCount, DirectionalLightLayout, _descriptorPool);

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		_lightDescriptorSetGroup.UpdateUniformBuffer(_vulkan, i, _directionalLightBuffers.Get(i)->Buffer, 0);
		_lightDescriptorSetGroup.UpdateUniformBuffer(_vulkan, i, _ambientLightBuffers.Get(i)->Buffer, 1);
	}
}

void ModelPipeline::CreateLightViewProjDescriptorSets()
{
	uint32_t imageCount = _vulkan->GetSwapchainImageCount();

	/* === CREATE LightViewProj BUFFERS === */

	_lightViewProjBuffers.Create(
		_vulkan,
		imageCount,
		sizeof(ViewProj),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	/* === ALLOCATE DESCRIPTOR SETS === */

	_lightViewProjDescriptorSetGroup.Allocate(_vulkan, imageCount, LightViewProjLayout, _descriptorPool);

	/* === WRITE DESCRIPTOR SETS === */

	for (int i = 0; i < imageCount; i++)
	{
		_lightViewProjDescriptorSetGroup.UpdateUniformBuffer(_vulkan, i, _lightViewProjBuffers.Get(i)->Buffer, 0);
	}
}

void ModelPipeline::Update(Camera* camera, ViewProj viewProjMatrix)
{
	// update viewproj
	_vulkan->CopyToMemory(_viewProjBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(viewProjMatrix), &viewProjMatrix);

	// update directional light
	_vulkan->CopyToMemory(_directionalLightBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(DirectionalLight), DirLight);

	// update ambient light
	AmbLight.CameraPosition = camera->Transform.GetPosition();
	_vulkan->CopyToMemory(_ambientLightBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(AmbientLight), &AmbLight);

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

	// update light viewproj
	Vec3 X = Vec3(0, 1, 0).Cross(DirLight->Direction).Normalized();
	Vec3 Y = DirLight->Direction.Cross(X).Normalized();

	Mat4 rotationMatrix;

	rotationMatrix.Set(0, 0, X.x);
	rotationMatrix.Set(1, 0, X.y);
	rotationMatrix.Set(2, 0, X.z);

	rotationMatrix.Set(0, 1, Y.x);
	rotationMatrix.Set(1, 1, Y.y);
	rotationMatrix.Set(2, 1, Y.z);

	rotationMatrix.Set(0, 2, DirLight->Direction.x);
	rotationMatrix.Set(1, 2, DirLight->Direction.y);
	rotationMatrix.Set(2, 2, DirLight->Direction.z);

	rotationMatrix.Set(3, 3, 1);

	Quaternion camrot = Quaternion::FromMatrix(rotationMatrix);
	Quaternion q1 = Quaternion::Euler(Math::Rad(-45.0f), Vec3(1, 0, 0));
	Quaternion q2 = Quaternion::Euler(Math::Rad(-45.0f), Vec3(0, 1, 0));
	Quaternion q = q2 * q1;

	auto campos = camera->Transform.GetPosition();
	Mat4 view = Math::Matrices::Translate(0, 0, 0);
	view = q.GetMatrix().Multiply(view);
	//view = Math::Matrices::Identity();
	view.Transpose();

	Mat4 proj = Math::Matrices::Orthographic(4096, 4096, 6.0f);
	//Mat4 proj = Math::Matrices::Perspective(1920, 1080, 60.0f, 0.01f, 100.0f);
	proj.Transpose();

	ViewProj camViewProj;
	camViewProj.View = view;
	camViewProj.Projection = proj;

	_vulkan->CopyToMemory(_lightViewProjBuffers.Get(_vulkan->_currentImage)->Memory, 0, sizeof(camViewProj), &camViewProj);
}

void ModelPipeline::RecordCommands(ViewProj viewProjMatrix)
{
	bool startRenderPass = true;
	bool endRenderPass = false;
	if (startRenderPass)
	{
		VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		VkClearValue clearDepth = { 1.0f, 0.0f, 0.0f, 0.0f };
		VkClearValue clearValues[] = { clearColor, clearDepth };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = _vulkan->_renderPass;
		renderPassBeginInfo.framebuffer = _vulkan->_swapchainFramebuffers[_vulkan->_currentImage];
		renderPassBeginInfo.renderArea.extent = _vulkan->_extent;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(*_vulkan->GetMainCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

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

	vkCmdBindDescriptorSets(
		*_vulkan->GetMainCommandBuffer(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_pipelineLayout,
		4,
		1,
		&_lightViewProjDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
		0,
		nullptr
	);

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
			&_modelDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
			1,
			&offset
		);

		// draw model meshes
		for (int j = 0; j < model->Drawables.size(); j++)
		{
			// bind material properties
			vkCmdBindDescriptorSets(
				*_vulkan->GetMainCommandBuffer(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_pipelineLayout,
				6,
				1,
				&model->Drawables[j]->Material->MaterialPropertiesDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
				0,
				nullptr
			);

			// bind color map
			vkCmdBindDescriptorSets(
				*_vulkan->GetMainCommandBuffer(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_pipelineLayout,
				2,
				1,
				&model->Drawables[j]->Material->ColorMap->DescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
				0,
				nullptr
			);
			
			// bind normal map
			if (model->Drawables[j]->Material->NormalMap != nullptr)
			{
				vkCmdBindDescriptorSets(
					*_vulkan->GetMainCommandBuffer(),
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					_pipelineLayout,
					5,
					1,
					&model->Drawables[j]->Material->NormalMap->DescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
					0,
					nullptr
				);
			}

			_vulkan->DrawMesh(
				*_vulkan->GetMainCommandBuffer(),
				&model->Drawables[j]->Mesh->VertexBuffer,
				&model->Drawables[j]->Mesh->IndexBuffer,
				model->Drawables[j]->Mesh->Indices.size()
			);
		}
	}

	if (endRenderPass)
	{
		vkCmdEndRenderPass(*_vulkan->GetMainCommandBuffer());
	}
}