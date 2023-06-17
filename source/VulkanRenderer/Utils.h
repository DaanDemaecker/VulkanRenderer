#pragma once
#include "stdafx.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>

namespace Utils
{
	std::vector<char> readFile(const std::string& filename);
}
