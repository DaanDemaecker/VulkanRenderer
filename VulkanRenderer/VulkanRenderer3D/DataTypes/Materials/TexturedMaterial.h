// TexturedMaterial.h
// This class will be used to bind textures to a modle
// The amount of textures can be decided by the user

#ifndef TexturedMaterialIncluded
#define TexturedMaterialIncluded

// File includes
#include "Material.h"

// Standard library includes
#include <initializer_list>
#include <string>

namespace D3D
{
	class TextureDescriptorObject;

	class TexturedMaterial final : public Material
	{
	public:
		// Constructor
		// Parameters:
		//     filePaths: an initializer list of the filepats to the textures in the order they should be for the shaders
		//     pipelineName: the name of the graphics pipeline that should be used in this material
		TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName = "Default");
		
		// Destructor
		virtual ~TexturedMaterial() = default;

		// Create the descriptorsets
		// Parameters:
		//     pModel: the model the descriptorsets are bound to
		//     descriptorSets: the descriptorsets that have to be created
		virtual void CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets) override;
		
		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: a vector of the descriptorsets that have to be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects) override;

	private:
		std::unique_ptr<TextureDescriptorObject> m_pDescriptorObject{};

		// The sampler for the textures
		VkSampler m_TextureSampler{};


		// Create the texture sampler
		void CreateTextureSampler();
	};
}
#endif // !TexturedMaterialIncluded