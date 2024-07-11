// DescriptorObject.h
// This class will serve as the base class for descriptor object classes
// Descriptor object classes will be needed to automate descriptor set updating

#ifndef DescriptorBufferIncluded
#define DescriptorBufferIncluded

// File includes
#include "../../Includes/VulkanIncludes.h"

// Standard libary includes
#include <vector>

namespace D3D
{
	class DescriptorObject
	{
	public:
		// Constructor
		// Parameters:
		//     type: the type of descriptor this object will ohld
		DescriptorObject(VkDescriptorType type)
			:m_Type{ type }
		{

		}

		// Delete default constructor
		DescriptorObject() = delete;

		// Default destructor
		virtual ~DescriptorObject() = default;

		// Delete all copy and move functions
		DescriptorObject(DescriptorObject& other) = delete;
		DescriptorObject(DescriptorObject&& other) = delete;

		DescriptorObject& operator=(DescriptorObject& other) = delete;
		DescriptorObject& operator=(DescriptorObject&& other) = delete;

		// Add the descriptor write objects to the list of descriptorWrites
		// Parameters:
		//     descriptorSet: the current descriptorset connected to this descriptor object
		//     descriptorWrites: the list of descriptorWrites this function will add to
		//     binding: the current binding in the shader files
		//     index: the current frame index of the renderer
		virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int index) = 0;

	protected:
		// The type of descriptor this object will hold
		VkDescriptorType m_Type{};

	};
}
#endif // !DescriptorBufferIncluded