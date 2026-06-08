// VulkanDescriptorSetLayout.h
// Helper class to create and manage a Vulkan descriptor set layout

#ifndef _DDM_VULKAN_DESCRIPTOR_LAYOUT_
#define _DDM_VULKAN_DESCRIPTOR_LAYOUT_

// Parent include
#include "Vulkan/Core/VulkanObject.h"
#include "Vulkan/Shaders/SpirVReflect/spirv_reflect.h"

namespace DDM::Vulkan
{
	// Class forward declarations
	class VulkanShaderModule;

	class VulkanDescriptorSetLayout final : public VulkanObject
	{
	public:
		// Delete default constructor
		VulkanDescriptorSetLayout() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pCore">Pointer to the VulkanCore instance</param>
		/// <param name="pAllocator">Pointer to the VulkanAllocator instance</param>
		/// <param name="pShaderModule">Pointer to the VulkanShaderModule instance to read layout from</param>
		VulkanDescriptorSetLayout(const VulkanCore* pCore, const VulkanAllocator* pAllocator, const VulkanShaderModule* pShaderModule);

		/// <summary>
		/// Destructor
		/// </summary>
		~VulkanDescriptorSetLayout();

		/// <summary>
		/// Get the Vulkan descriptor set layout
		/// </summary>
		/// <returns>Vulkan descriptor set layout</returns>
		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_VkDescriptorSetLayout; }
	private:
		VkDescriptorSetLayout m_VkDescriptorSetLayout{ VK_NULL_HANDLE };

		/// <summary>
		/// Create the cescriptorset layout from the reflected shader module
		/// </summary>
		/// <param name="pShaderModule">Pointer to the VulkanShaderModule instance to read layout from</param>
		void CreateDescriptorSetLayout(const VulkanShaderModule* pShaderModule);

		/// <summary>
		/// Read the bindings for the descriptor set layout from the reflected shader module and add them to the given vector of bindings
		/// </summary>
		/// <param name="pShaderModule">Const ref to the refelcted shader module to read layout from</param>
		/// <param name="bindings">Vector to store the descriptor set layout bindings</param>
		void ReadBindings(const SpvReflectShaderModule& pShaderModule, std::vector<VkDescriptorSetLayoutBinding>& bindings);
	};
}

#endif // !_DDM_VULKAN_DESCRIPTOR_LAYOUT_
