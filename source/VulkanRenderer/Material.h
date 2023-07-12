#ifndef MaterialIncluded
#define MaterialIncluded


namespace D3D
{
	class Material
	{
	public:
		Material(const std::string& filePath, const std::string& pipelineName = "Default");
		~Material();

		Material() = delete;

		Material(Material& other) = delete;
		Material(Material&& other) = delete;

		Material& operator=(Material& other) = delete;
		Material& operator=(Material&& other) = delete;
	
		VkImageView& GetImageView() { return m_TextureImageView; }
		VkSampler& GetSampler() { return m_TextureSampler; }
		PipelinePair& GetPipeline() { return m_PipelinePair; }

	private:
		VkImage m_TextureImage{};
		VkDeviceMemory m_TextureImageMemory{};

		VkImageView m_TextureImageView{};
		VkSampler m_TextureSampler{};

		uint32_t m_MipLevels{};

		PipelinePair m_PipelinePair{};

		//Initialization functions
		void CreateTextureImage(const std::string& filePath);
		void CreateTextureImageView();
		void CreateTextureSampler();
	};
}
#endif // !MaterialIncluded