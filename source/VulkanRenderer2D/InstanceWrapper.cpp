// InstanceWrapper.cpp

// File includes
#include "InstanceWrapper.h"

// Standard library includes
#include <stdexcept>

D2D::InstanceWrapper::InstanceWrapper()
{
	CreateInstance();
}

void D2D::InstanceWrapper::CreateInstance()
{
	VkInstanceCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	VkApplicationInfo applicationInfo{};
	SetupApplicationInfo(applicationInfo);
	createInfo.pApplicationInfo = &applicationInfo;


	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}
}

void D2D::InstanceWrapper::SetupApplicationInfo(VkApplicationInfo& /*info*/)
{
}
