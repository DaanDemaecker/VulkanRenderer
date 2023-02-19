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
	GLFWwindow* m_pWindow;
	const uint32_t m_Widht;
	const uint32_t m_Height;

	//Vulkan instance
	VkInstance m_Instance;

	//Debug messenger
	VkDebugUtilsMessengerEXT m_DebugMessenger;

	const std::vector<const char*> m_ValidationLayers = {
	"VK_LAYER_KHRONOS_validation" };



	//Member functions
	void initWindow();

	void initVulkan();

	void createInstance();

	void mainLoop();

	void cleanup();


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
