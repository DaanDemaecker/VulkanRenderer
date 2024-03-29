// TextureDescriptorObject.cpp

//File includes
#include "TextureDescriptorObject.h"
#include "VulkanRenderer3D.h"

D3D::TextureDescriptorObject::TextureDescriptorObject(Texture&& texture)
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	m_Textures.push_back(texture);
}

D3D::TextureDescriptorObject::TextureDescriptorObject(std::initializer_list<const std::string>& filePaths)
	:DescriptorObject(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
	SetupTextures(filePaths);

	SetupImageInfos();

}

void D3D::TextureDescriptorObject::AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int /*index*/)
{
	descriptorWrites.resize(binding + m_Textures.size());

	for (auto& imageInfo : m_ImageInfos)
	{
		auto& currentBinding{ descriptorWrites[binding] };

		currentBinding.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

		currentBinding.dstBinding = binding;
		currentBinding.dstArrayElement = 0;
		currentBinding.descriptorType = m_Type;
		currentBinding.descriptorCount = 1;
		currentBinding.pImageInfo = &imageInfo;
		currentBinding.dstSet = descriptorSet;

		binding++;
	}
}

void D3D::TextureDescriptorObject::SetupTextures(std::initializer_list<const std::string>& filePaths)
{
	// Resize textures to textureAmount
	m_Textures.resize(filePaths.size());

	// Initialize index variable
	int index{};

	auto& renderer{ VulkanRenderer3D::GetInstance() };

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
	m_ImageInfos.resize(m_Textures.size());
	
	auto& sampler{ VulkanRenderer3D::GetInstance().GetSampler() };

	int index{};

	for (auto& texture : m_Textures)
	{
		m_ImageInfos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_ImageInfos[index].imageView = texture.imageView;
		m_ImageInfos[index].sampler = sampler;

		index++;
	}
}
