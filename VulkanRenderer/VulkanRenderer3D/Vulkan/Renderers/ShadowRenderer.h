#ifndef ShadowRendererIncluded
#define ShadowRendererIncluded

// File includes
#include "../../Includes/VulkanIncludes.h"
#include "../../DataTypes/Structs.h"
#include "../../DataTypes/DescriptorObjects/TextureDescriptorObject.h"

// Standard library includes
#include <memory>

namespace D3D
{
	class GPUObject;
	class ImageManager;
	class PipelineWrapper;
	class Model;
	class Viewport;

	class ShadowRenderer final
	{
	public:
		// Constructor
		// Parameters:
		//     msaaSamples: max amount of samples per pixel
		ShadowRenderer(GPUObject* pGPUObject, VkExtent2D swapchainExtent);

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

		void Render(std::vector<std::unique_ptr<Model>>& pModels, VkExtent2D swapchainExtent);

		void CreatePipeline(VkDevice device);

		TextureDescriptorObject* GetTextureDescriptorObject();

	private:
		// Max amount of samples per pixel, initialize as 1
		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

		Texture m_ShadowTexture{};

		std::unique_ptr<D3D::TextureDescriptorObject> m_pShadowTextureObject{};

		VkFramebuffer m_ShadowFrameBuffer{};

		VkRenderPass m_ShadowRenderpass{};

		std::unique_ptr<PipelineWrapper> m_pShadowPipeline{};

		uint16_t m_ShadowMapSize{2048};

		// Viewport object
		std::unique_ptr<Viewport> m_pViewport{};

		// Vector of descriptorsets
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		bool m_DescriptorSetInitialized{ false };

		// Initialize the depth image for the swapchain
		// Parameters:
		//     pGPUObject: pointer to the GPU Object
		//     pImageManager: pointer to the image manager
		void CreateDepthImage(GPUObject* pGPUObject, VkExtent2D swapchainExtent);


		// Create the frame buffers
		// Parameters:
		//     device: handle of the VkDevice
		void CreateFramebuffers(VkDevice device, VkExtent2D swapchainExtent);

		void CreateRenderPass(VkDevice device);

		void SetupDescriptorSets();

		void CreateDescriptorSets();

		void UpdateDescriptorSets();

		void RecreateFrameBuffer();
	};
}

#endif // !ShadowRendererIncluded