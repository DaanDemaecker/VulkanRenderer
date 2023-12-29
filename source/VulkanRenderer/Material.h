#ifndef MaterialIncluded
#define MaterialIncluded

#include "Structs.h"
#include <iostream>

namespace D3D
{
	class DescriptorPoolWrapper;
	class Model;

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

		virtual void CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets);

		virtual void UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets);

		virtual VkDescriptorSetLayout* GetDescriptorLayout();

		virtual DescriptorPoolWrapper* GetDescriptorPool();

	protected:
		PipelinePair m_PipelinePair{};
	};
}
#endif // !MaterialIncluded