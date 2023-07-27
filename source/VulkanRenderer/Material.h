#ifndef MaterialIncluded
#define MaterialIncluded


namespace D3D
{
	class Material
	{
	public:
		Material(const std::string& pipelineName = "Default");
		virtual ~Material() = default;

		Material(Material& other) = delete;
		Material(Material&& other) = delete;

		Material& operator=(Material& other) = delete;
		Material& operator=(Material&& other) = delete;

		PipelinePair& GetPipeline() { return m_PipelinePair; }

		virtual void UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets);

	protected:
		PipelinePair m_PipelinePair{};
	};
}
#endif // !MaterialIncluded