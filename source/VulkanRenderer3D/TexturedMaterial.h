// TexturedMaterial.h
// This class will be used to bind textures to a modle
// The amount of textures can be decided by the user

#ifndef TexturedMaterialIncluded
#define TexturedMaterialIncluded

// File includes
#include "Material.h"

// Standard library includes
#include <initializer_list>
#include <iostream>

namespace D3D
{
	class TexturedMaterial final : public Material
	{
	public:
		// Constructor
		// Parameters:
		//     filePaths: an initializer list of the filepats to the textures in the order they should be for the shaders
		//     pipelineName: the name of the graphics pipeline that should be used in this material
		TexturedMaterial(std::initializer_list<const std::string> filePaths, const std::string& pipelineName = "Default");
		
		// Destructor
		~TexturedMaterial();

		// Create the descriptorsets
		// Parameters:
		//     pModel: the model the descriptorsets are bound to
		//     descriptorSets: the descriptorsets that have to be created
		virtual void CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets) override;
		
		// Update the descriptorsets
		// Parameters:
		//     uboBuffers: a vector of the Uniform buffer objects
		//     descriptorsets: a vector of the descriptorsets that have to be updated
		virtual void UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets) override;

		// Get the descriptorsetlayout used in this material
		virtual std::vector<VkDescriptorSetLayout>& GetDescriptorLayout() override;

		// Get the descriptorpoolwrapper associated with this material
		virtual DescriptorPoolWrapper* GetDescriptorPool() override;

	private:
		// The amount of textures this material uses
		int m_TextureAmount{};

		// Vector of the VkImage objects that hold the textures
		std::vector<VkImage> m_TextureImages{};
		// Vector of VkDeviceMemory obects that hold the textures
		std::vector<VkDeviceMemory> m_TextureImageMemories{};
		// Vector of the VkImageView objects
		std::vector<VkImageView> m_TextureImageViews{};
		// The sampler for the textures
		VkSampler m_TextureSampler{};
		// The amount of levels the mipmap will have
		uint32_t m_MipLevels{};

		// Create texture image
		// Parameters:
		//     filePath: the filepath to the image that will be used for the texture
		//     index: the index of the current image within the vector of VKImages
		void CreateTextureImage(const std::string& filePath, int index);

		// Create texture image view
		// Parameters:
		//     index: the index of the current image within the vector of VkImageViews
		void CreateTextureImageView(int index);

		// Create the texture sampler
		void CreateTextureSampler();
	};
}
#endif // !TexturedMaterialIncluded