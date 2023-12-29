#ifndef DescriptorPoolManagerIncluded
#define DescriptorPoolManagerIncluded

#include "GLFWIncludes.h"

#include <utility>
#include <map>
#include <memory>

namespace D3D
{
	class DescriptorPoolWrapper;

	class DescriptorPoolManager
	{
	public:
		DescriptorPoolManager() = default;
		~DescriptorPoolManager() = default;

		DescriptorPoolManager(DescriptorPoolManager& other) = delete;
		DescriptorPoolManager(DescriptorPoolManager&& other) = delete;

		DescriptorPoolManager& operator=(DescriptorPoolManager& other) = delete;
		DescriptorPoolManager& operator=(DescriptorPoolManager&& other) = delete;

		void Cleanup(VkDevice device);

		D3D::DescriptorPoolWrapper* GetDescriptorPool(uint32_t uboAmount, uint32_t textureAmount);
		

	private:
		std::pair<uint32_t, uint32_t> m_Pair{ std::make_pair<uint32_t, uint32_t>(0, 0) };


		std::map<std::pair<uint32_t, uint32_t>, std::unique_ptr<DescriptorPoolWrapper>> m_pDescriptorPools;
	};
}

#endif // !DescriptorPoolManagerIncluded