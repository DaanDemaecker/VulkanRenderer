// CubeMapMaterial.h
// This class will serve as the material of the cube map and it will hold all the textures for the cubemap

// File includes
#include "Material.h"

namespace D3D
{
	class TextureDescriptorObject;

	class CubeMapMaterial final : public Material
	{
	public:
		// Constructor
		// Parameters:
		//     filePaths: list of filepaths to the textures
		CubeMapMaterial(const std::initializer_list<const std::string>&& filePaths);

		// Constructor
		// Parameters:
		//     filePaths: list of filepaths to the textures
		CubeMapMaterial(const std::initializer_list<const std::string>& filePaths);

		// Destructor
		~CubeMapMaterial();

		CubeMapMaterial(CubeMapMaterial& other) = delete;
		CubeMapMaterial(CubeMapMaterial&& other) = delete;

		CubeMapMaterial& operator=(CubeMapMaterial& other) = delete;
		CubeMapMaterial& operator=(CubeMapMaterial&& other) = delete;

		// Create the descriptorsets
		// Parameters:
		//     pModel: the model the descriptorsets are bound to
		//     descriptorSets: the descriptorsets that have to be created
		virtual void CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets) override;

		// Update the descriptorsets
		// Parameters:
		//     uboBuffers: a vector of the Uniform buffer objects
		//     descriptorsets: a vector of the descriptorsets that have to be updated
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects) override;

	private:
		// Texture struct
		Texture m_CubeTexture{};

		std::unique_ptr<TextureDescriptorObject> m_pDescriptorObject{};

		// The amount of levels the mipmap will have
		uint32_t m_MipLevels{};
	};
}

