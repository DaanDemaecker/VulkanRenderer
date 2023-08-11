#ifndef TexturedMaterialIncluded
#define TexturedMaterialIncluded
#include "Material.h"
#include <initializer_list>

namespace D3D
{
	class TexturedMaterial final : public Material
	{
	public:
		TexturedMaterial(std::initializer_list<const std::string> filePaths, const std::string& pipelineName = "Default");
		~TexturedMaterial();

		PipelinePair& GetPipeline() { return m_PipelinePair; }

		virtual void UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets) override;

		virtual VkDescriptorSetLayout* GetDescriptorLayout() override;

	private:
		std::vector<VkImage> m_TextureImages{};
		std::vector<VkDeviceMemory> m_TextureImageMemories{};

		std::vector<VkImageView> m_TextureImageViews{};
		VkSampler m_TextureSampler{};

		uint32_t m_MipLevels{};

		//Initialization functions
		void CreateTextureImage(const std::string& filePath, int index);
		void CreateTextureImageView(int index);
		void CreateTextureSampler();
	};
}
#endif // !TexturedMaterialIncluded