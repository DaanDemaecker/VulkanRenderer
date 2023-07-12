#include "stdafx.h"
#include "Utils.h"
#include <iostream>

#ifndef  TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 26495)
#pragma warning(disable : 26498)
#pragma warning(disable : 26451)
#include <tiny_obj_loader.h>
#pragma warning(pop)
#endif

std::vector<char> Utils::readFile(const std::string& filename)

{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file " + filename);
	}

	//Because of the std::ios::ate flag, the file will be read from the end, getting the current position in the file will therefore give its size
	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	//Now that we have the size of the file, we can move back to the start and store the data into the buffer
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void Utils::LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	vertices.clear();
	indices.clear();

	tinyobj::attrib_t attrib{};
	std::vector<tinyobj::shape_t> shapes{};
	std::vector<tinyobj::material_t> materials{};

	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[static_cast<uint64_t>(3) * index.vertex_index],
				attrib.vertices[static_cast<uint64_t>(3) * index.vertex_index + static_cast<uint64_t>(1)],
				attrib.vertices[static_cast<uint64_t>(3) * index.vertex_index + static_cast<uint64_t>(2)]
			};

			vertex.texCoord = {
				attrib.texcoords[static_cast<uint64_t>(2) * index.texcoord_index],
				1.0f - attrib.texcoords[static_cast<uint64_t>(2) * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[static_cast<uint64_t>(3) * index.normal_index],
				attrib.normals[static_cast<uint64_t>(3) * index.normal_index + static_cast<uint64_t>(1)],
				attrib.normals[static_cast<uint64_t>(3) * index.normal_index + static_cast<uint64_t>(2)]
			};

			vertex.color = { 1.0f, 0.0f, 0.0f };

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}
