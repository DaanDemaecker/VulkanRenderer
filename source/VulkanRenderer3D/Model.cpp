// Model.cpp

// File includes
#include "Model.h"
#include "VulkanRenderer3D.h"
#include "Utils.h"
#include "Material.h"
#include "TimeManager.h"
#include "DescriptorPoolWrapper.h"
#include "PipelineWrapper.h"

// Standard library includes
#include <memory>

D3D::Model::Model()
{
	// Crate default material as placeholder
	m_pMaterial = std::make_shared<D3D::Material>();
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
{
	auto& renderer{ VulkanRenderer3D::GetInstance() };

	// Check if model is initialized, if it is, clean up first
	if (m_Initialized)
	{
		m_Initialized = false;
		Cleanup();
	}

	// Load model into vertex and index vectors
	Utils::LoadModel(textPath, m_Vertices, m_Indices);
	// Create vertex buffer
	renderer.CreateVertexBuffer(m_Vertices, m_VertexBuffer, m_VertexBufferMemory);
	// Create index buffer
	renderer.CreateIndexBuffer(m_Indices, m_IndexBuffer, m_IndexBufferMemory);
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

	// Check if UBOs have changed, if so, update them
	//if (m_UboChanged[frame])
	//{
		UpdateUniformBuffer(frame);
	//}

	// Render model
	renderer.Render(this, renderer.GetCurrentCommandBuffer(), &m_DescriptorSets[frame], GetPipeline());
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

	// Get size of Uniform Buffer Object
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	// Resize ubobuffers to amount of frames
	m_UboBuffers.resize(frames);
	// Resize ubo memory to amount of frames
	m_UbosMemory.resize(frames);
	// Resize mapped memory to amount of frames
	m_UbosMapped.resize(frames);
	// Resize ubos to amount of frames
	m_Ubos.resize(frames);
	// Resize dirty flages to amount of frames
	m_UboChanged.resize(frames);

	// Set dirty flag
	SetDirtyFlags();

	// Loop for the amount of frames there are
	for (size_t i = 0; i < frames; ++i)
	{
		// Create memory
		renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_UboBuffers[i], m_UbosMemory[i]);

		// Map memory from uboMemory to ubosmapped
		vkMapMemory(renderer.GetDevice(), m_UbosMemory[i], 0, bufferSize, 0, &m_UbosMapped[i]);
	}
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
	// Update descriptorsets
	m_pMaterial->UpdateDescriptorSets(m_UboBuffers, m_DescriptorSets);
}

void D3D::Model::UpdateUniformBuffer(uint32_t frame)
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

	// Update ubo
	// Send to renderer to update camera matrix
	VulkanRenderer3D::GetInstance().UpdateUniformBuffer(m_Ubos[frame]);

	// Reset dirty flag
	m_UboChanged[frame] = false;

	// Copy memory from ubos to ubosmapped
	memcpy(m_UbosMapped[frame], &m_Ubos[frame], sizeof(m_Ubos[frame]));
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

	// Destroy index buffer
	vkDestroyBuffer(device, m_IndexBuffer, nullptr);
	// Free index buffer memory
	vkFreeMemory(device, m_IndexBufferMemory, nullptr);

	// Destroy vertex buffer
	vkDestroyBuffer(device, m_VertexBuffer, nullptr);
	// Free vertex buffer
	vkFreeMemory(device, m_VertexBufferMemory, nullptr);

	// Loop for the amount of frames
	for (size_t i = 0; i < renderer.GetMaxFrames(); ++i)
	{
		// Destroy uboBuffers
		vkDestroyBuffer(device, m_UboBuffers[i], nullptr);
		// Free ubo buffer memory
		vkFreeMemory(device, m_UbosMemory[i], nullptr);
	}
}

void D3D::Model::SetDirtyFlags()
{
	// Set all dirty flags
	std::fill(m_UboChanged.begin(), m_UboChanged.end(), true);
}
