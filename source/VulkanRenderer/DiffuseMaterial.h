#ifndef DiffuseMaterialIncluded
#define DiffuseMaterialIncluded
#include "Material.h"

namespace D3D
{
	class DiffuseMaterial final : public Material
	{
	public:
		DiffuseMaterial(const std::string& filePath, const std::string& pipelineName = "Default");
		~DiffuseMaterial();

		PipelinePair& GetPipeline() { return m_PipelinePair; }

		virtual void UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets) override;

	private:
		VkImage m_TextureImage{};
		VkDeviceMemory m_TextureImageMemory{};

		VkImageView m_TextureImageView{};
		VkSampler m_TextureSampler{};

		uint32_t m_MipLevels{};

		//Initialization functions
		void CreateTextureImage(const std::string& filePath);
		void CreateTextureImageView();
		void CreateTextureSampler();
	};
}
#endif // !DiffuseMaterialIncluded