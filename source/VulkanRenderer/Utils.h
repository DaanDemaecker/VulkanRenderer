#ifndef UtilsIncluded
#define UtilsIncluded

#include "stdafx.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>

namespace Utils
{
	std::vector<char> readFile(const std::string& filename);

	void LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
}

#endif // !UtilsIncluded