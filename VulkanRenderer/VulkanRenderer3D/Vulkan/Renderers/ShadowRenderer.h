#ifndef ShadowRendererIncluded
#define ShadowRendererIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "DataTypes/Structs.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"

// Standard library includes
#include <memory>

namespace DDM3
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
		ShadowRenderer();

		// Destructor
		~ShadowRenderer();

		// Delete move and copy functions
		ShadowRenderer(ShadowRenderer& other) = delete;
		ShadowRenderer(ShadowRenderer&& other) = delete;

		ShadowRenderer& operator=(ShadowRenderer& other) = delete;
		ShadowRenderer& operator=(ShadowRenderer&& other) = delete;

		void Render(std::vector<std::unique_ptr<Model>>& pModels);

		void CreatePipeline(VkDevice device);

		TextureDescriptorObject* GetTextureDescriptorObject();

	private:
		// Max amount of samples per pixel, initialize as 1
		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

		Texture m_ShadowTexture{};

		std::unique_ptr<DDM3::TextureDescriptorObject> m_pShadowTextureObject{};

		VkFramebuffer m_ShadowFrameBuffer{};

		VkRenderPass m_ShadowRenderpass{};

		std::unique_ptr<PipelineWrapper> m_pShadowPipeline{};

		const uint16_t m_ShadowMapSize;

		// Viewport object
		std::unique_ptr<Viewport> m_pViewport{};

		// Vector of descriptorsets
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		bool m_DescriptorSetInitialized{ false };

		// Initialize the depth image for the swapchain
		void CreateDepthImage();


		// Create the frame buffers
		// Parameters:
		//     device: handle of the VkDevice
		void CreateFramebuffers(VkDevice device);

		void CreateRenderPass(VkDevice device);

		void SetupDescriptorSets();

		void CreateDescriptorSets();

		void UpdateDescriptorSets();

		void RecreateFrameBuffer();


		void Cleanup(VkDevice device);
	};
}

#endif // !ShadowRendererIncluded