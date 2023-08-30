#ifndef DescriptorPoolWrapperIncluded
#define DescriptorPoolWrapperIncluded

#include "stdafx.h"

namespace D3D
{
	class DescriptorPoolWrapper
	{
	public:
		DescriptorPoolWrapper() = default;
		~DescriptorPoolWrapper() = default;

		DescriptorPoolWrapper(DescriptorPoolWrapper& other) = delete;
		DescriptorPoolWrapper(DescriptorPoolWrapper&& other) = delete;

		DescriptorPoolWrapper& operator=(DescriptorPoolWrapper& other) = delete;
		DescriptorPoolWrapper& operator=(DescriptorPoolWrapper&& other) = delete;

		VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }

	private:
		uint32_t m_UboAmount{};
		uint32_t m_TextureAmount{};

		int m_MaxDescriptorSets{8};
		int m_IncreaseFactor{ 2 };


		VkDescriptorPool m_DescriptorPool{};

	};
}

#endif // !DescriptorPoolWrapperIncluded