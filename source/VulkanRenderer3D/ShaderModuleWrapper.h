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

	private:
		std::vector<char> m_ShaderCode{};

		VkShaderModule m_ShaderModule{};

		VkPipelineShaderStageCreateInfo m_ShaderstageCreateInfo{};

		SpvReflectShaderModule m_ReflectShaderModule{};

		void CreateShaderModule(VkDevice device);

		void ReflectShader();

		void CreateShaderStageInfo();
	};
}

#endif // !ShaderModuleWrapperIncluded