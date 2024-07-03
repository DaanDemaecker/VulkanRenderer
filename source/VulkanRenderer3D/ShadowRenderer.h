#ifndef ShadowRendererIncluded
#define ShadowRendererIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"
#include "UboDescriptorObject.h"

// Standard library includes
#include <memory>

namespace D3D
{
	class GPUObject;
	class ImageManager;
	class PipelineWrapper;

	class ShadowRenderer final
	{
	public:
		// Constructor
		// Parameters:
		//     msaaSamples: max amount of samples per pixel
		ShadowRenderer(GPUObject* pGPUObject, VkSampleCountFlagBits msaaSamples,
			D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer);

		// Delete default constructor
		ShadowRenderer() = delete;

		// Dafault destructor
		~ShadowRenderer() = default;

		// Delete move and copy functions
		ShadowRenderer(ShadowRenderer& other) = delete;
		ShadowRenderer(ShadowRenderer&& other) = delete;

		ShadowRenderer& operator=(ShadowRenderer& other) = delete;
		ShadowRenderer& operator=(ShadowRenderer&& other) = delete;

		void Cleanup(VkDevice device);

		void Render();

		void CreatePipeline(VkDevice device);

	private:
		// Max amount of samples per pixel, initialize as 1
		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

		Texture m_ShadowTexture{};

		VkFramebuffer m_ShadowFrameBuffer{};

		VkRenderPass m_ShadowRenderpass{};

		std::unique_ptr<PipelineWrapper> m_pShadowPipeline{};

		uint32_t m_ShadowMapSize{ 1024 };

		// Descriptor set variables
		std::unique_ptr<UboDescriptorObject<glm::mat4>> m_pLightProjectionObject{};

		// Vector of descriptorsets
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		bool m_DescriptorSetInitialized{ false };

		// Initialize the depth image for the swapchain
		// Parameters:
		//     pGPUObject: pointer to the GPU Object
		//     pImageManager: pointer to the image manager
		void CreateDepthImage(GPUObject* pGPUObject, ImageManager* pImageManager, VkCommandBuffer commandBuffer);


		// Create the frame buffers
		// Parameters:
		//     device: handle of the VkDevice
		void CreateFramebuffers(VkDevice device);

		void CreateRenderPass(VkDevice device);

		void SetupDescriptorSets();

		void CreateDescriptorSets();

		void UpdateDescriptorSets();
	};
}

#endif // !ShadowRendererIncluded