#pragma once
#include "stdafx.h"

class Renderer {
public:
	Renderer(const uint32_t width, const uint32_t height)
		:m_Widht{ width }, m_Height{ height }
	{

	}

	Renderer(Renderer& rhs) = delete;
	Renderer(Renderer&& rhs) = delete;

	Renderer& operator=(Renderer& rhs) = delete;
	Renderer& operator=(Renderer&& rhs) = delete;

	

	void run();


private:
	//Memeber variables
	//Window and dimensions
	GLFWwindow* m_pWindow{};
	const uint32_t m_Widht;
	const uint32_t m_Height;

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

	const std::vector<const char*> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
	
	//Surface
	VkSurfaceKHR m_Surface{};

	//Present queue
	VkQueue m_PresentQueue{};

	//DeviceExtensions
	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	//SwapChain
	VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

	std::vector<VkImage> m_SwapChainImages;

	VkFormat m_SwapChainImageFormat{};

	VkExtent2D m_SwapChainExtent{};


	//Member functions
	void initWindow();

	void initVulkan();

	void createInstance();

	void mainLoop();

	void cleanup();

	//Create vulkan surface
	void CreateSurface();

	//Logic to find optimal physical device
	void pickPhysicalDevice();

	//Logic to find graphics queue family
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	//Creating the logical device
	void createLogicalDevice();

	//Creating the swapchain
	void createSwapChain();

	//Functions for checking suitability of the swap chain
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	//Functions for getting the right swapchain settings
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


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
};

