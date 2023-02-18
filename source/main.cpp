#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}
private:
	//Memeber variables
	GLFWwindow* m_pWindow;


	//Member functions
	void initWindow()
	{
		//Initialize glfw
		glfwInit();

		//Tell GLFW not to create an OpenGL context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//Tell GLFW window shouldn't be resizable
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		//Initialize the window
		m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	void initVulkan()
	{

	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(m_pWindow))
		{
			glfwPollEvents();
		}
	}

	void cleanup()
	{
		glfwDestroyWindow(m_pWindow);

		glfwTerminate();
	}
};

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}