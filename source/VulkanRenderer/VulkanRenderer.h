#ifndef VulkanRendererIncluded
#define VulkanRendererIncluded

#include "Singleton.h"

namespace D3D
{
    class Model;

    class VulkanRenderer final : public Singleton<VulkanRenderer>
    {
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        VulkanRenderer(VulkanRenderer& other) = delete;
        VulkanRenderer(VulkanRenderer&& other) = delete;

        VulkanRenderer& operator=(VulkanRenderer& other) = delete;
        VulkanRenderer& operator=(VulkanRenderer&& other) = delete;

        void Render(std::vector<std::unique_ptr<Model>>& pModels);

        void Render(Model* pModel, VkCommandBuffer& commandBuffer, const VkDescriptorSet* descriptorSet, const PipelinePair& pipeline);
        
        void AddGraphicsPipeline(const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName);

        //Public getters
        VkDevice& GetDevice() {return m_Device; }
        VkCommandPool& GetCommandPool() { return m_CommandPool; }
        VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
        VkDescriptorPool& GetDescriptorPool() { return m_DescriptorPool; }
        VkImageView& GetDefaultImageView() { return m_DefaultTextureImageView; }
        VkSampler& GetSampler() { return m_TextureSampler; }
        PipelinePair& GetPipeline(const std::string& name = "Default");

        //Public Helpers
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
            VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

        void UpdateUniformBuffer(UniformBufferObject& buffer);

    private:
        //----Member variables----
        //---Validation layers---
        bool m_EnableValidationLayers{true};
        const std::vector<const char*> m_ValidationLayers =
        { 
            "VK_LAYER_KHRONOS_validation"
        };
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;


        //---Vulkan Instance---
        VkInstance m_Instance = VK_NULL_HANDLE;


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

        //-Present queue-
        VkQueue m_PresentQueue{};

        //--Swapchain--
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

        //-Swapchain Images-
        std::vector<VkImage> m_SwapChainImages{};

        //-Swapchain Image Format-
        VkFormat m_SwapChainImageFormat{};

        //-Swapchain Extent-
        VkExtent2D m_SwapChainExtent{};

        //--Image Views--
        std::vector<VkImageView> m_SwapChainImageViews{};

        //--Renderpass--
        VkRenderPass m_RenderPass{};

        //-RenderpassInfo-
        VkRenderPassBeginInfo m_RenderpassInfo{};

        //--Descriptorlayout--
        VkDescriptorSetLayout m_DescriptorSetLayout{};

        //--GraphicsPipeline--
        //VkPipeline m_GraphicsPipeline{};
        std::map<std::string, PipelinePair> m_GraphicPipelines{};

        const std::string m_DefaultPipelineName{"Default"};
        const std::string m_DefaultVertName{ "../resources/DefaultResources/Default.Vert.spv" };
        const std::string m_DefaultFragName{ "../resources/DefaultResources/Default.Frag.spv" };

        //--CommandPool--
        VkCommandPool m_CommandPool{};

        //CommandBuffer
        std::vector<VkCommandBuffer> m_CommandBuffers{};
       

        //--MultiSampling--
        VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkImage m_ColorImage{};
        VkDeviceMemory m_ColorImageMemory{};
        VkImageView m_ColorImageView{};

        //--Depth Image--
        VkImage m_DepthImage{};
        VkDeviceMemory m_DepthImageMemory{};
        VkImageView m_DepthImageView{};

        //--Framebuffers--
        std::vector<VkFramebuffer> m_SwapChainFramebuffers{};

        //--Descriptorpool--
        VkDescriptorPool m_DescriptorPool{};
        uint32_t m_MaxDescriptorSets{8};

        //--Sync objects--
        std::vector<VkSemaphore> m_ImageAvailableSemaphores{};
        std::vector<VkSemaphore> m_RenderFinishedSemaphores{};
        std::vector<VkFence> m_InFlightFences{};


        //--Current frame--
        uint32_t m_CurrentFrame = 0;


        //DefaultTexture
        VkImage m_DefaultTextureImage{};
        VkDeviceMemory m_DefaultTextureImageMemory{};
        VkImageView m_DefaultTextureImageView{};
        uint32_t m_MipLevels{};

        const std::string m_DefaultTextureName{ "../resources/DefaultResources/DefaultTexture.png" };
        


        VkSampler m_TextureSampler{};


        //----Member Functions----
        //--- Cleanup ---
        void Cleanup();
         
        
        //---Vulkan Initialization---
        void InitVulkan();

        //--Vulkan instance--
        void CreateInstance();

        //-Validation layers--
        bool CheckValidationLayerSupport();
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );
        void SetupDebugMessenger();
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessegerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        //-Extensions-
        std::vector<const char*> GetRequiredExtensions();

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

        //--RenderPass--
        void CreateRenderPass();

        //--Descriptor Layout
        void CreateDescriptorLayout();

        //-Shader Module-
        VkShaderModule CreateShaderModule(const std::vector<char>& code);

        //--Command Pool
        void CreateCommandPool();

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

        //--DescriptorPool--
        void CreateDescriptorPool();

        //--CommandBuffers--
        void CreateCommandBuffers();
        void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex, std::vector<std::unique_ptr<Model>>& pModels);

        //--Sync Objects--
        void CreateSyncObjects();
        

        //--General helpers--
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        
        bool HasStencilComponent(VkFormat format);


        //Texture functions
        void CreateTextureImage();
        void CreateTextureImageView();
        void CreateTextureSampler();


        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer comandBuffer);
        
    };
}

#endif // !VulkanRendererIncluded