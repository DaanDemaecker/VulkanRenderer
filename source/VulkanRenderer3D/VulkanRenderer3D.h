#ifndef VulkanRendererIncluded
#define VulkanRendererIncluded

#include "Singleton.h"
#include "Structs.h"

#include "PipelineManager.h"

#include <memory>
#include <vector>
#include <map>
#include <iostream>


namespace D3D
{
    class Model;
    class DescriptorPoolManager;
    class ImGuiWrapper;
    class InstanceWrapper;
    class ImageManager;
    class CommandpoolManager;
    class RenderpassWrapper;
    class SwapchainWrapper;

        class VulkanRenderer3D final : public Singleton<VulkanRenderer3D>
    {
    public:
        VulkanRenderer3D();
        ~VulkanRenderer3D();

        VulkanRenderer3D(VulkanRenderer3D& other) = delete;
        VulkanRenderer3D(VulkanRenderer3D&& other) = delete;

        VulkanRenderer3D& operator=(VulkanRenderer3D& other) = delete;
        VulkanRenderer3D& operator=(VulkanRenderer3D&& other) = delete;

        void Render(std::vector<std::unique_ptr<Model>>& pModels);

        void Render(Model* pModel, VkCommandBuffer& commandBuffer, const VkDescriptorSet* descriptorSet, const PipelinePair& pipeline);

        void AddGraphicsPipeline(const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName, int vertexUbos, int fragmentUbos, int textureAmount);

        //Public getters
        size_t GetMaxFrames() const { return m_MaxFramesInFlight; }
        VkDevice& GetDevice() { return m_Device; }
        VkImageView& GetDefaultImageView();
        VkSampler& GetSampler();
        PipelinePair& GetPipeline(const std::string& name = "Default");
        VkCommandBuffer& GetCurrentCommandBuffer();
        uint32_t GetCurrentFrame() const { return  m_CurrentFrame; }
        DescriptorPoolManager* GetDescriptorPoolManager() const;
        const DirectionalLightObject& GetGlobalLight() const { return m_GlobalLight; }
        std::vector<VkBuffer>& GetLightBuffers() { return m_LightBuffers; }


        //Public Helpers
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void UpdateUniformBuffer(UniformBufferObject& buffer);

        std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayout(int vertexUbos, int fragmentUbos, int textureAmount);

        void CreateTexture(Texture& texture, const std::string& textureName, uint32_t& mipLevels);

    private:
        std::unique_ptr<DescriptorPoolManager> m_pDescriptorPoolManager{};
        std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};
        std::unique_ptr<PipelineManager> m_pPipelineManager{};
        std::unique_ptr<InstanceWrapper> m_pInstanceWrapper{};
        std::unique_ptr<ImageManager> m_pImageManager{};
        std::unique_ptr<CommandpoolManager> m_pCommandPoolManager{};
        std::unique_ptr<RenderpassWrapper> m_pRenderpassWrapper{};
        std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

        const size_t m_MaxFramesInFlight{ 2 };

        DirectionalLightObject m_GlobalLight{};
        std::vector<bool> m_LightChanged{};

        std::vector<VkBuffer> m_LightBuffers{};
        std::vector<VkDeviceMemory> m_LightMemory{};
        std::vector<void*> m_LightMapped{};

        //----Member variables----
        //---Validation layers---
        bool m_EnableValidationLayers{ true };
        const std::vector<const char*> m_ValidationLayers =
        {
            "VK_LAYER_KHRONOS_validation"
        };


        //--Window Surface--
        VkSurfaceKHR m_Surface{};


        //--Physical Device--
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

        //-Physical Device Extensions-
        const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        //--Logical device--
        VkDevice m_Device = VK_NULL_HANDLE;

        //-Graphics queue-
        VkQueue m_GraphicsQueue{};

        //-Graphics queue index-
        uint32_t m_GraphicsQueueIndex{};

        //-Present queue-
        VkQueue m_PresentQueue{};

        const std::string m_DefaultPipelineName{ "Default" };
        const std::string m_DefaultVertName{ "../resources/DefaultResources/Default.Vert.spv" };
        const std::string m_DefaultFragName{ "../resources/DefaultResources/Default.Frag.spv" };

        //--Sync objects--
        std::vector<VkSemaphore> m_ImageAvailableSemaphores{};
        std::vector<VkSemaphore> m_RenderFinishedSemaphores{};
        std::vector<VkFence> m_InFlightFences{};


        //--Current frame--
        uint32_t m_CurrentFrame = 0;

        //----Member Functions----
        //--- Cleanup ---
        void CleanupVulkan();
         
        //---Cleanup Imgui---
        void CleanupImGui();
        
        //---Vulkan Initialization---
        void InitVulkan();

        //---IMGUI Initialization---
        void InitImGui();

        //--Surface creation--
        void CreateSurface();

        //--Physical device--
        void PickPhysicalDevice();

        //-Physical device helper functions-
        bool IsDeviceSuitable(VkPhysicalDevice device);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

        //--Logical Device--
        void CreateLogicalDevice();

        //-Swapchain Helper Functions-
        void RecreateSwapChain();

        //-MultiSampling Helpers-
        VkSampleCountFlagBits GetMaxUsableSampleCount();

        //-Depth Image Helper-
        VkFormat FindDepthFormat();

        //--CommandBuffers--
        void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex, std::vector<std::unique_ptr<Model>>& pModels);

        //--Sync Objects--
        void CreateSyncObjects();

        //--LightBuffers--
        void CreateLightBuffer();
        void UpdateLightBuffer(int frame);

        //--General helpers--
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice);
    };
}

#endif // !VulkanRendererIncluded