#pragma once
#include "stdafx.h"

class HelloTriangleApplication {
public:
	HelloTriangleApplication(const uint32_t width, const uint32_t height)
		:m_Widht{ width }, m_Height{ height }
	{

	}

	HelloTriangleApplication(HelloTriangleApplication& rhs) = delete;
	HelloTriangleApplication(HelloTriangleApplication&& rhs) = delete;

	HelloTriangleApplication& operator=(HelloTriangleApplication& rhs) = delete;
	HelloTriangleApplication& operator=(HelloTriangleApplication&& rhs) = delete;

	

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


	//Debug messenger
	VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

	const std::vector<const char*> m_ValidationLayers = {
	"VK_LAYER_KHRONOS_validation" };



	//Member functions
	void initWindow();

	void initVulkan();

	void createInstance();

	void mainLoop();

	void cleanup();

	//Logic to find optimal physical device
	void pickPhysicalDevice();

	//Logic to find graphics queue family
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);


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
};
