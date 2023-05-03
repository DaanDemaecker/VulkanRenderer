#pragma once
#include "stdafx.h"

class Renderer {
public:
	Renderer(int width, int height);
	~Renderer();

	Renderer(Renderer& rhs) = delete;
	Renderer(Renderer&& rhs) = delete;

	Renderer& operator=(Renderer& rhs) = delete;
	Renderer& operator=(Renderer&& rhs) = delete;

	bool WindowShouldClose();

	void DrawFrame();

	void RecreateSwapChain();

	static void framebufferResizeCallback(GLFWwindow* pWindow, int width, int height);

private:
	//Memeber variables
	//Window and dimensions
	GLFWwindow* m_pWindow{};
	int m_Widht;
	int m_Height;

	const std::vector<Vertex> m_Vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	//Vulkan instance
	VkInstance m_Instance = VK_NULL_HANDLE;

	//Physical device
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

	//Logical device
	VkDevice m_Device = VK_NULL_HANDLE;

	//Graphics queue
	VkQueue m_GraphicsQueue{};

	//Debug messenger
	VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

	const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	//Surface
	VkSurfaceKHR m_Surface{};

	//Present queue
	VkQueue m_PresentQueue{};

	//DeviceExtensions
	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	//SwapChain
	VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

	std::vector<VkImage> m_SwapChainImages{};

	VkFormat m_SwapChainImageFormat{};

	VkExtent2D m_SwapChainExtent{};

	//Images
	std::vector<VkImageView> m_SwapChainImageViews{};

	//Renderpass
	VkRenderPass m_RenderPass{};

	//PipelineLayout
	VkPipelineLayout m_PipeLineLayout{};

	//GraphicsPipeline
	VkPipeline m_GraphicsPipeline{};

	//Swapchainframebuffers
	std::vector<VkFramebuffer> m_SwapChainFramebuffers{};

	//CommandPool
	VkCommandPool m_CommandPool{};

	//CommandBuffer
	std::vector<VkCommandBuffer> m_CommandBuffers{};

	//Sync objects
	std::vector<VkSemaphore> m_ImageAvailableSemaphores{};
	std::vector<VkSemaphore> m_RenderFinishedSemaphores{};
	std::vector<VkFence> m_InFlightFences{};

	//Resize flag
	bool m_FrameBufferResized = false;

	//Current frame
	uint32_t m_CurrentFrame = 0;

	//Vertex buffer
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};


	//Member functions
	void initWindow();

	void initVulkan();

	void createInstance();

	void cleanup();

	//Create vulkan surface
	void CreateSurface();

	//Logic to find optimal physical device
	void pickPhysicalDevice();

	//Logic to find graphics queue family
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	//Creating the logical device
	void createLogicalDevice();

	//Creating and cleaning up the swapchain
	void createSwapChain();
	void cleanupSwapChain();

	//Functions for checking suitability of the swap chain
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	//Functions for getting the right swapchain settings
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	//ImageView
	void createImageViews();

	//Graphics pipeline
	void createGraphicsPipeline();

	//Shader modules
	VkShaderModule createShaderModule(const std::vector<char>& code);

	//RenderPass
	void createRenderPass();

	//Framebuffers
	void createFramebuffers();

	//Commandpool
	void createCommandPool();

	//CommandBuffer
	void createCommandBuffers();

	void recordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

	//Sync objects
	void createSyncObjects();

	//Buffer
	void createVertexBuffer();
	
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);



	//Validation layers functions
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void setupDebugMessenger();

	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
		);

	//Helper functions 
	bool IsDeviceSuitable(VkPhysicalDevice device);

	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessegerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

