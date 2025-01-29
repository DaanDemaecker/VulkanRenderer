// Model.cpp

// Header include
#include "Model.h"

// File includes
#include "Engine/TimeManager.h"

#include "Utils/Utils.h"

#include "DataTypes/Materials/Material.h"

#include "Mesh.h"

#include "Vulkan/Vulkan3D.h"
#include "Vulkan/Wrappers/DescriptorPoolWrapper.h"
#include "Vulkan/Wrappers/PipelineWrapper.h"

// Standard library includes
#include <memory>

DDM3::Model::Model()
{
	// Crate default material as placeholder
	m_pMaterial = std::make_shared<DDM3::Material>();

	m_pUboDescriptorObject = std::make_unique<DDM3::UboDescriptorObject<UniformBufferObject>>();
}

DDM3::Model::~Model()
{
	// Check if model is initialized, if it is, clean it up
	if (m_Initialized)
	{
		Cleanup();
	}
}

void DDM3::Model::LoadModel(const std::string& textPath)
{// Check if model is initialized, if it is, clean up first
	if (m_Initialized)
	{
		m_Initialized = false;
		Cleanup();
	}

	m_pMesh = std::make_unique<Mesh>(textPath);

	// Create uniform buffer
	CreateUniformBuffers();
	// Create descriptorsets
	CreateDescriptorSets();

	// Set initialized to true
	m_Initialized = true;
}

void DDM3::Model::SetMaterial(std::shared_ptr<Material> pMaterial)
{
	// Remove model from old descriptorpool
	m_pMaterial->GetDescriptorPool()->RemoveModel(this);
	// Set new material
	m_pMaterial = pMaterial;
	// Create new descriptorpool
	CreateDescriptorSets();
}

void DDM3::Model::Update()
{
	if (m_Rotate)
	{
		// Initialize rotation speed
		constexpr float rotSpeed{ -glm::radians(15.f) };

		// Calculate amount of rotation
		float rotAmount{ rotSpeed * TimeManager::GetInstance().GetDeltaTime() };

		// Set new rotation
		SetRotation(m_Rotation.x, m_Rotation.y + rotAmount, m_Rotation.z);
	}
}

void DDM3::Model::RenderShadow(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
	if (!m_CastsShadow)
		return;

	auto frame{ Vulkan3D::GetCurrentFrame()};

	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &m_Ubos[frame].model);

	m_pMesh->Render(commandBuffer);
}

void DDM3::Model::Render()
{
	// If model isn't initialize, return
	if (!m_Initialized)
		return;

	// Get reference to renderer
	auto& renderer{ Vulkan3D::GetInstance().GetRenderer()};
	// Get index of current frame
	auto frame{ Vulkan3D::GetCurrentFrame() };

	UpdateUniformBuffer(frame);



	// Get current commandbuffer
	auto commandBuffer{ renderer.GetCurrentCommandBuffer() };

	// Bind pipeline
	GetPipeline()->BindPipeline(commandBuffer);

	// Bind descriptor sets
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline()->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frame], 0, nullptr);


	m_pMesh->Render(commandBuffer);

}

void DDM3::Model::SetPosition(float x, float y, float z)
{
	// Set new position
	m_Position = { x, y, z };
	// Set dirty flags
	SetDirtyFlags();
}

void DDM3::Model::SetRotation(float x, float y, float z)
{
	// Set new rotation
	m_Rotation = { x, y, z };
	// Set dirty flags
	SetDirtyFlags();
}

void DDM3::Model::SetScale(float x, float y, float z)
{
	// Set new scale
	m_Scale = { x, y, z };
	// Set dirty flags
	SetDirtyFlags();
}

void DDM3::Model::CreateUniformBuffers()
{
	// Get amount of frames
	auto frames = Vulkan3D::GetMaxFrames();

	// Resize ubos to amount of frames
	m_Ubos.resize(frames);
	// Resize dirty flages to amount of frames
	m_UboChanged.resize(frames);

	// Set dirty flag
	SetDirtyFlags();
}

void DDM3::Model::CreateDescriptorSets()
{
	// Create descriptorsets
	m_pMaterial->CreateDescriptorSets(this, m_DescriptorSets);
	// Update descriptors
	UpdateDescriptorSets();
}

void DDM3::Model::UpdateDescriptorSets()
{
	std::vector<DescriptorObject*> descriptors{ m_pUboDescriptorObject.get() };

	// Update descriptorsets
	m_pMaterial->UpdateDescriptorSets(m_DescriptorSets, descriptors);
}

void DDM3::Model::UpdateUniformBuffer(uint32_t frame)
{

	if (m_UboChanged[frame])
	{
		// Get translation matrix
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_Position);

		// Convert rotation to quaternion
		glm::quat quaternion = glm::quat(m_Rotation);
		// Get rotation matrix
		glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);

		// Get scaling matrix
		glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), m_Scale);

		// Set Ubo
		m_Ubos[frame].model = translationMatrix * rotationMatrix * scalingMatrix;

		// Reset dirty flag
		m_UboChanged[frame] = false;
	}

	// Update ubo
	// Send to renderer to update camera matrix
	Vulkan3D::GetInstance().GetCurrentCamera()->UpdateUniformBuffer(m_Ubos[frame]);

	m_pUboDescriptorObject->UpdateUboBuffer(m_Ubos[frame], frame);
}

DDM3::PipelineWrapper* DDM3::Model::GetPipeline()
{
	// Check if material exist, if not, return default
	if (m_pMaterial != nullptr)
	{
		return m_pMaterial->GetPipeline();
	}

	return Vulkan3D::GetInstance().GetRenderer().GetPipeline();
}

void DDM3::Model::Cleanup()
{
	// Get reference to device
	auto device = DDM3::Vulkan3D::GetInstance().GetDevice();

	// Wait until device is idle
	vkDeviceWaitIdle(device);

	m_pMesh = nullptr;
}

void DDM3::Model::SetDirtyFlags()
{
	// Set all dirty flags
	std::fill(m_UboChanged.begin(), m_UboChanged.end(), true);
}
