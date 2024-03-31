// ShaderModuleWrapper.h
// This class will load in a shader and use SpirV reflect to get the information needed from the code

#ifndef ShaderModuleWrapperIncluded
#define ShaderModuleWrapperIncluded

// File includes
#include "VulkanIncludes.h"
#include "spirv_reflect.h"

// Standard library includes
#include <string>
#include <vector>
#include <map>


namespace D3D
{
	class ShaderModuleWrapper
	{
	public:
		ShaderModuleWrapper() = default;

		ShaderModuleWrapper(VkDevice device, const std::string& filePath);

		~ShaderModuleWrapper() = default;

		ShaderModuleWrapper(ShaderModuleWrapper& other) = delete;
		ShaderModuleWrapper(ShaderModuleWrapper&& other) = delete;

		ShaderModuleWrapper& operator=(ShaderModuleWrapper& other) = delete;
		ShaderModuleWrapper& operator=(ShaderModuleWrapper&& other) = delete;

		void Cleanup(VkDevice device);

		VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const { return m_ShaderstageCreateInfo; }

		void AddDescriptorSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& bindings);

		void AddDescriptorTypeCount(std::map<VkDescriptorType, int>& typeCount);
	private:
		// The binary code from the shader
		std::vector<char> m_ShaderCode{};

		// The vulkan shader module
		VkShaderModule m_ShaderModule{};

		// The createInfo for the shader stage
		VkPipelineShaderStageCreateInfo m_ShaderstageCreateInfo{};

		// The shade info from spirv-reflect
		SpvReflectShaderModule m_ReflectShaderModule{};

		// Create a vulkan shader module from binary code
		// Parameters:
		//     device: handle of the VkDevice
		void CreateShaderModule(VkDevice device);

		// Read in the shader information with SpirV-reflect
		void ReflectShader();

		// Create the info for the shader stage
		void CreateShaderStageInfo();
	};
}

#endif // !ShaderModuleWrapperIncluded