// TextureDescriptorObject.cpp

//File includes
#include "TextureDescriptorObject.h"
#include "Vulkan/Vulkan3D.h"

D3D::TextureDescriptorObject::TextureDescriptorObject(Texture& texture)
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	// Add the texture to the list of textures
	m_Textures.push_back(texture);

	// Set up the image infos
	SetupImageInfos();
}

D3D::TextureDescriptorObject::TextureDescriptorObject(std::initializer_list<const std::string>& filePaths)
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	// Set up all textures
	SetupTextures(filePaths);

	// Set up the image infos
	SetupImageInfos();
}

D3D::TextureDescriptorObject::~TextureDescriptorObject()
{
	// Get the device and clean up all the textures

	auto device{ Vulkan3D::GetInstance().GetDevice() };

	for (auto& texture : m_Textures)
	{
		texture.Cleanup(device);
	}
}

void D3D::TextureDescriptorObject::AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int /*index*/)
{
	// Resize the descriptor writes vector
	descriptorWrites.resize(binding + m_Textures.size());

	// Loop trough all the image infos
	for (auto& imageInfo : m_ImageInfos)
	{
		// Create new binding
		auto& currentBinding{ descriptorWrites[binding] };

		currentBinding.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

		// Set all fields of current binding with correct values
		currentBinding.dstBinding = binding;
		currentBinding.dstArrayElement = 0;
		currentBinding.descriptorType = m_Type;
		currentBinding.descriptorCount = 1;
		currentBinding.pImageInfo = &imageInfo;
		currentBinding.dstSet = descriptorSet;

		// Increase the binding index
		binding++;
	}
}

D3D::Texture& D3D::TextureDescriptorObject::GetTexture(int index)
{
	{ return m_Textures[index]; }
}

void D3D::TextureDescriptorObject::SetupTextures(std::initializer_list<const std::string>& filePaths)
{
	// Resize textures to textureAmount
	m_Textures.resize(filePaths.size());

	// Initialize index variable
	int index{};

	auto& renderer{ Vulkan3D::GetInstance().GetRenderer()};

	// Loop trough all filePaths
	for (const auto& path : filePaths)
	{
		// Create texture
		renderer.CreateTexture(m_Textures[index], path);

		// Increment index
		++index;
	}
}

void D3D::TextureDescriptorObject::SetupImageInfos()
{
	// resize image infos
	m_ImageInfos.resize(m_Textures.size());
	
	// Get the sampler
	auto& sampler{ Vulkan3D::GetInstance().GetRenderer().GetSampler()};

	int index{};

	// Loop trough all the textures
	for (auto& texture : m_Textures)
	{
		// Set image layout to shader read optimal
		m_ImageInfos[index].imageLayout = texture.layout;
		// Set correct image view
		m_ImageInfos[index].imageView = texture.imageView;
		// Set sampler
		m_ImageInfos[index].sampler = sampler;

		index++;
	}
}
