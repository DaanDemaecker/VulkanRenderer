#include "SurfaceWrapper.h"
#include "Engine/Window.h"

#include <stdexcept>

DDM3::SurfaceWrapper::SurfaceWrapper(VkInstance instance)
{
	// Create the surface
	CreateSurface(instance);
}

void DDM3::SurfaceWrapper::Cleanup(VkInstance instance)
{
	// Destroy the surface
	vkDestroySurfaceKHR(instance, m_Surface, nullptr);
}

void DDM3::SurfaceWrapper::CreateSurface(VkInstance instance)
{
	// Create the window surface
	if (glfwCreateWindowSurface(instance, Window::GetInstance().GetWindowStruct().pWindow, nullptr, &m_Surface) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create window surface!");
	}
}
