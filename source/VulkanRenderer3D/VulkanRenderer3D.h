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
        std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper;
        std::unique_ptr<PipelineManager> m_pPipelineManager;
        std::unique_ptr<InstanceWrapper> m_pInstanceWrapper;
        std::unique_ptr<ImageManager> m_pImageManager;
        std::unique_ptr<CommandpoolManager> m_pCommandPoolManager;
        std::unique_ptr<RenderpassWrapper> m_pRenderpassWrapper;

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

        //--Swapchain--
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

        //-Swapchain MinImageCount-
        uint32_t m_MinImageCount{};

        //-Swapchain Images-
        std::vector<VkImage> m_SwapChainImages{};

        //-Swapchain Image Format-
        VkFormat m_SwapChainImageFormat{};

        //-Swapchain Extent-
        VkExtent2D m_SwapChainExtent{};

        //-Image Views-
        std::vector<VkImageView> m_SwapChainImageViews{};

        const std::string m_DefaultPipelineName{ "Default" };
        const std::string m_DefaultVertName{ "../resources/DefaultResources/Default.Vert.spv" };
        const std::string m_DefaultFragName{ "../resources/DefaultResources/Default.Frag.spv" };


        //--MultiSampling--
        VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
        Texture m_ColorImage{};

        //--Depth Image--
        Texture m_DepthImage{};

        //--Framebuffers--
        std::vector<VkFramebuffer> m_SwapChainFramebuffers{};
        

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
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        //--Logical Device--
        void CreateLogicalDevice();

        //--Swapchain--
        void CreateSwapChain();

        //-Swapchain Helper Functions-
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        void CleanupSwapChain();
        void RecreateSwapChain();

        //--Swapchain Image Views--
        void CreateImageViews();

        //--MultiSampling--
        void CreateColorResources();

        //-MultiSampling Helpers-
        VkSampleCountFlagBits GetMaxUsableSampleCount();

        //--Depth Image--
        void CreateDepthResources();

        //-Depth Image Helper-
        VkFormat FindDepthFormat();

        //--Framebuffers--
        void CreateFramebuffers();

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

    };
}

#endif // !VulkanRendererIncluded