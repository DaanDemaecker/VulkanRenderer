// DescriptorObject.h
// This class will serve as the base class for descriptor object classes
// Descriptor object classes will be needed to automate descriptor set updating

#ifndef DescriptorBufferIncluded
#define DescriptorBufferIncluded

// File includes
#include "VulkanIncludes.h"

// Standard libary includes
#include <vector>

namespace D3D
{
	class DescriptorObject
	{
	public:
		DescriptorObject(VkDescriptorType type)
			:m_Type{ type }
		{

		}

		DescriptorObject() = delete;

		virtual ~DescriptorObject() = default;

		DescriptorObject(DescriptorObject& other) = delete;
		DescriptorObject(DescriptorObject&& other) = delete;

		DescriptorObject& operator=(DescriptorObject& other) = delete;
		DescriptorObject& operator=(DescriptorObject&& other) = delete;

		virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int index) = 0;

	protected:
		VkDescriptorType m_Type{};

	};
}
#endif // !DescriptorBufferIncluded