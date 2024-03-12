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
		TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName = "Default");
		
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

		// Vector of textures
		std::vector<Texture> m_Textures{};

		// The sampler for the textures
		VkSampler m_TextureSampler{};
		// The amount of levels the mipmap will have
		uint32_t m_MipLevels{};

		// Create the texture sampler
		void CreateTextureSampler();
	};
}
#endif // !TexturedMaterialIncluded