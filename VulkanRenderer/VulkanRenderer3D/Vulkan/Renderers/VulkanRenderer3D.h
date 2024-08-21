// VulkanRenderer3D.h
// This file will act as the main renderer, it holds all the objects needed for rendering

#ifndef VulkanRendererIncluded
#define VulkanRendererIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <memory>
#include <vector>
#include <map>
#include <string>


namespace D3D
{
    // Class forward declarations
    class Vulkan3D;
    class Model;
    class ImGuiWrapper;
    class ImageManager;
    class CommandpoolManager;
    class RenderpassWrapper;
    class SwapchainWrapper;
    class SyncObjectManager;
    class DirectionalLightObject; 
    class SkyBox;
    class BufferManager;
    class PipelineWrapper;
    class DescriptorObject;
    class Camera;
    class PipelineManager;
    class Viewport;
    class ShadowRenderer;
    class TextureDescriptorObject;

    // Inherit from singleton
    class VulkanRenderer3D final
    {
    friend class D3D::Vulkan3D;

    public:
        // Constructor
        VulkanRenderer3D();
        // Destructor
        ~VulkanRenderer3D();

        // Delete copy and move functions
        VulkanRenderer3D(VulkanRenderer3D& other) = delete;
        VulkanRenderer3D(VulkanRenderer3D&& other) = delete;
        VulkanRenderer3D& operator=(VulkanRenderer3D& other) = delete;
        VulkanRenderer3D& operator=(VulkanRenderer3D&& other) = delete;

        // Add a new graphics pipeline
        // Parameters:
        //     pipelineName: the name of the new pipeline
        //     filePaths: a list of shader file names for this pipeline
        //     hasDepthStencil: boolean that indicates if this pipeline needs a depth stencil, true by default
        void AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths,
            bool hasDepthStencil = true);

        //Get the default image view
        VkImageView& GetDefaultImageView();

        // Get the image sampler
        VkSampler& GetSampler();

        // Get the pipeline with the given name
        // Parameters:
        //     name: the name of the requested pipeline, "Default" by default
        PipelineWrapper* GetPipeline(const std::string& name = "Default");

        // Get the commandbuffer currently in use
        VkCommandBuffer& GetCurrentCommandBuffer();

        // Get the global light object
        const DirectionalLightStruct& GetGlobalLightStruct() const;

        // Get the global light object
        DirectionalLightObject* GetGlobalLight() const;


        // Create a buffer
        // Parameters:
        //     size: the size of the buffer
        //     usage: the usage flags for the buffer
        //     properties: the property flags for the buffer
        //     buffer: reference to the buffer to be made
        //     bufferMemory: reference to the memory of the buffer that will be made
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        // Copy a buffer to another buffer
        // Parameters:
        //     srcBuffer: the source buffer
        //     dstBuffer: the destination buffer
        //     size: the size of the buffers
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        // Create a vertex buffer
        // Parameters:
        //     vertices: reference to vector of vertices 
        //     vertexBuffer: handle to the vertex buffer to be created
        //     vertexBufferMemory: handle of the vertex buffer memory
        void CreateVertexBuffer(std::vector<D3D::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);

        // Create a vertex buffer
        // Parameters:
        //     indices: reference to vector of indices 
        //     indexBuffer: handle to the index buffer to be created
        //     indexBufferMemory: handle of the index buffer memory
        void CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);

        // Create a texture
        // Parameters:
        //     texture: reference to the texture object that will hold the texture
        //     textureName: textpath to the image
        void CreateTexture(Texture& texture, const std::string& textureName);

        // Create a cube texture
        // Parameters:
        //     cubeTexture: reference to the texture object
        //     textureNames: a list of the file paths for the cube faces in order: right,left,up,down,front,back
        void CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<std::string const>& textureNames);

        // Set up the skybox
        void SetupSkybox();

        // Set up the global light
        void SetupLight();

        // Initialize the default pipeline
        void SetupDefaultPipeline();

        TextureDescriptorObject* GetShadowMapDescriptorObject();

        // Transition an image from one layout to another
        // Parameters:
        //     image: the image that will be transitioned
        //     format: the format the image is in
        //     oldLayout: the current layout of the image
        //     newLayout: the desired layout of the image
        //     mipLevels: the amount of mipmaps in the image
        //     layerCount: the amount of images stored in the single VkImage object, 1 by default
        void TransitionImageLayout(VkImage image, VkFormat format,
            VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount = 1);

        VkExtent2D GetSwapchainExtent() const;
    private:
        std::unique_ptr<ShadowRenderer> m_pShadowRenderer{};

        // Viewport object
        std::unique_ptr<Viewport> m_pViewport{};

        // Pointer to the buffer manager
        std::unique_ptr<BufferManager> m_pBufferManager{};

        // Pointer to the ImGui wrapper
        std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};

        // Pointer to thep pipeline manager
        std::unique_ptr<PipelineManager> m_pPipelineManager{};

        // Pointer to the image manager
        std::unique_ptr<ImageManager> m_pImageManager{};

        // Pointer to the commandpool manager
        std::unique_ptr<CommandpoolManager> m_pCommandPoolManager{};

        // Pointer to the renderpass wrapper
        std::unique_ptr<RenderpassWrapper> m_pRenderpassWrapper{};

        // Pointer to the swapchain wrapper
        std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

        // Pointer to the sync object manager
        std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

        // Pointer to the global light object
        std::unique_ptr<DirectionalLightObject> m_pGlobalLight{};

        // Initialize vulkan objects
        void InitVulkan();

        // Initialize ImGui
        void InitImGui();

        // Function that will render all the given models
        // Parameters:
        //     pModels: list of models that have to be rendered
        void Render(std::vector<std::unique_ptr<Model>>& pModels);

        // Recreate the swapchain
        void RecreateSwapChain();

        // Record the command buffer
        // Parameter:
        //     commandBuffer: the current commandBuffer
        //     imageIndex: the current frame index
        //     pModels: list of models that will be rendered
        void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex, std::vector<std::unique_ptr<Model>>& pModels);

        // Begin a command buffer for a single command
        VkCommandBuffer BeginSingleTimeCommands();

        // End a command buffer after a single command
        // Parameters:
        //     commandBuffer: handle of the buffer to be ended
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

        // Transition an image frome one layout to another
        // Parameters:
        //     image: the given image
        //     format: the format of the image
        //     oldLayout: the old layout of the image
        //     newLayout: the new layout of the image
        //     mipLevels: the amount of mipmaps in this image
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    };
}

#endif // !VulkanRendererIncluded