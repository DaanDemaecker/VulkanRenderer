// Model.cpp

// File includes
#include "Model.h"
#include "VulkanRenderer3D.h"
#include "Utils.h"
#include "Material.h"
#include "TimeManager.h"
#include "DescriptorPoolWrapper.h"
#include "PipelineWrapper.h"
#include "Mesh.h"

// Standard library includes
#include <memory>

D3D::Model::Model()
{
	// Crate default material as placeholder
	m_pMaterial = std::make_shared<D3D::Material>();

	m_pUboDescriptorObject = std::make_unique<D3D::UboDescriptorObject<UniformBufferObject>>();
}

D3D::Model::~Model()
{
	// Check if model is initialized, if it is, clean it up
	if (m_Initialized)
	{
		Cleanup();
	}
}

void D3D::Model::LoadModel(const std::string& textPath)
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

void D3D::Model::SetMaterial(std::shared_ptr<Material> pMaterial)
{
	// Remove model from old descriptorpool
	m_pMaterial->GetDescriptorPool()->RemoveModel(this);
	// Set new material
	m_pMaterial = pMaterial;
	// Create new descriptorpool
	CreateDescriptorSets();
}

void D3D::Model::Update()
{
	// Initialize rotation speed
	constexpr float rotSpeed{-glm::radians(15.f)};

	// Calculate amount of rotation
	float rotAmount{rotSpeed * TimeManager::GetInstance().GetDeltaTime()};

	// Set new rotation
	SetRotation(m_Rotation.x, m_Rotation.y + rotAmount , m_Rotation.z);
}

void D3D::Model::Render()
{
	// If model isn't initialize, return
	if (!m_Initialized)
		return;

	// Get reference to renderer
	auto& renderer{ VulkanRenderer3D::GetInstance() };
	// Get index of current frame
	auto frame{ renderer.GetCurrentFrame() };

	UpdateUniformBuffer(frame);



	// Get current commandbuffer
	auto commandBuffer{ VulkanRenderer3D::GetInstance().GetCurrentCommandBuffer() };

	// Bind pipeline
	GetPipeline()->BindPipeline(commandBuffer);

	// Bind descriptor sets
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline()->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frame], 0, nullptr);


	m_pMesh->Render(commandBuffer);

}

void D3D::Model::SetPosition(float x, float y, float z)
{
	// Set new position
	m_Position = { x, y, z };
	// Set dirty flags
	SetDirtyFlags();
}

void D3D::Model::SetRotation(float x, float y, float z)
{
	// Set new rotation
	m_Rotation = { x, y, z };
	// Set dirty flags
	SetDirtyFlags();
}

void D3D::Model::SetScale(float x, float y, float z)
{
	// Set new scale
	m_Scale = { x, y, z };
	// Set dirty flags
	SetDirtyFlags();
}

void D3D::Model::CreateUniformBuffers()
{
	// Get reference to renderer
	auto& renderer = VulkanRenderer3D::GetInstance();
	// Get amount of frames
	auto frames = renderer.GetMaxFrames();

	// Resize ubos to amount of frames
	m_Ubos.resize(frames);
	// Resize dirty flages to amount of frames
	m_UboChanged.resize(frames);

	// Set dirty flag
	SetDirtyFlags();
}

void D3D::Model::CreateDescriptorSets()
{
	// Create descriptorsets
	m_pMaterial->CreateDescriptorSets(this, m_DescriptorSets);
	// Update descriptors
	UpdateDescriptorSets();
}

void D3D::Model::UpdateDescriptorSets()
{
	std::vector<DescriptorObject*> descriptors{ m_pUboDescriptorObject.get() };

	// Update descriptorsets
	m_pMaterial->UpdateDescriptorSets(m_DescriptorSets, descriptors);
}

void D3D::Model::UpdateUniformBuffer(uint32_t frame)
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
	VulkanRenderer3D::GetInstance().UpdateUniformBuffer(m_Ubos[frame]);

	m_pUboDescriptorObject->UpdateUboBuffer(m_Ubos[frame], frame);
}

D3D::PipelineWrapper* D3D::Model::GetPipeline()
{
	// Check if material exist, if not, return default
	if (m_pMaterial != nullptr)
	{
		return m_pMaterial->GetPipeline();
	}

	return VulkanRenderer3D::GetInstance().GetPipeline();
}

void D3D::Model::Cleanup()
{
	// Get reference to renderer
	auto& renderer = D3D::VulkanRenderer3D::GetInstance();
	// Get reference to device
	auto device = renderer.GetDevice();

	// Wait until device is idle
	vkDeviceWaitIdle(device);

	m_pMesh = nullptr;
}

void D3D::Model::SetDirtyFlags()
{
	// Set all dirty flags
	std::fill(m_UboChanged.begin(), m_UboChanged.end(), true);
}
