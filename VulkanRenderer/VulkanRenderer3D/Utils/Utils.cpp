// Utils.cpp

// Header include
#include "Utils.h"

// File includes
#include "Includes/TinyObjLoaderIncludes.h"

std::vector<char> Utils::readFile(const std::string& filename)
{
	// Open file
	std::ifstream file{ filename, std::ios::ate | std::ios::binary };

	// If file failed to open, throw runtime error
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file " + filename);
	}

	// Because of the std::ios::ate flag, the file will be read from the end, getting the current position in the file will therefore give its size
	size_t fileSize = static_cast<size_t>(file.tellg());

	// Char buffer created with size of the file
	std::vector<char> buffer(fileSize);

	// Now that we have the size of the file, we can move back to the start and store the data into the buffer
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	// Close the file
	file.close();

	// Return the buffer
	return buffer;
}


void Utils::LoadModel(const std::string& filename, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	// Clear the vectors in case they aren't empty
	vertices.clear();
	indices.clear();

	// Create needed objects to read in .obj file
	tinyobj::attrib_t attrib{};
	std::vector<tinyobj::shape_t> shapes{};
	std::vector<tinyobj::material_t> materials{};

	// Create objects for error throwing
	std::string err;

	// Read file, returned false, throw error
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str()))
	{
		throw std::runtime_error(filename + " is not a valid file path");
	}

	// Create map to store vertices
	std::unordered_map<D3D::Vertex, uint32_t> uniqueVertices{};

	// Loop through every shape that was read from the file
	for (const auto& shape : shapes)
	{
		// Loop through all indices in current shape
		for (const auto& index : shape.mesh.indices)
		{
			// Create empty vertex
			D3D::Vertex vertex{};

			// Add position to vertex
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (index.texcoord_index >= 0 && static_cast<size_t>(index.texcoord_index * 2 + 1) < attrib.texcoords.size())
			{
				// Add UV coords to vertex
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if (index.normal_index >= 0 && static_cast<size_t>(index.normal_index * 3 + 2) < attrib.normals.size())
			{
				// Add normal to vertex
				vertex.normal = {
					attrib.normals[3 * index.normal_index],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}
			else
			{
				// Default normal if not provided
				vertex.normal = { 0.0f, 0.0f, 0.0f };
			}

			// Add color to vertex
			vertex.color = { 1.0f, 1.0f, 1.0f };

			// If vertex isn't in uniqueVertices vector, add it
			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			// Add index to indices vector
			indices.push_back(uniqueVertices[vertex]);
		}
	}

	SetupTangents(vertices, indices);
}

void Utils::SetupTangents
(std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	// After all vertices are added loop through them to calculate the tangents
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		// Get the indices of the current triangle
		uint32_t index0 = indices[i];
		uint32_t index1 = indices[i + 1];
		uint32_t index2 = indices[i + 2];

		// Get the vertices associated with this triangle
		D3D::Vertex& v0 = vertices[index0];
		D3D::Vertex& v1 = vertices[index1];
		D3D::Vertex& v2 = vertices[index2];

		// Get 2 edges of this triangle
		glm::vec3 edge1 = v1.pos - v0.pos;
		glm::vec3 edge2 = v2.pos - v0.pos;

		// Get the difference in UV over these edges
		glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
		glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

		// Calculate the scaling factor for normalizing the vector
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		// Calculate the tangent
		glm::vec3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * r;

		// Add the tangent to the 3 vertices
		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;
	}

	// Normalize the tangents
	for (auto& vertex : vertices)
	{
		vertex.tangent = glm::normalize(vertex.tangent);
	}
}


std::string Utils::GetExtension(const std::string& filename)
{
	// Get the index of the final period in the name, all characters after it indicate the extension
	auto index = filename.find_last_of(".");

	return filename.substr(index + 1, filename.size());
}

glm::quat Utils::RotationFromDirection(const glm::vec3& direction)
{
	// Ensure the direction vector is normalized
	glm::vec3 normalizedDirection = glm::normalize(direction);

	// Define the default forward vector
	glm::vec3 defaultForward = glm::vec3(0.0f, 0.0f, 1.0f);

	// Calculate the dot product to find the angle
	float dot = glm::dot(defaultForward, normalizedDirection);

	// If the direction is the same as the default forward vector
	if (glm::abs(dot - 1.0f) < glm::epsilon<float>()) {
		return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	}
	// If the direction is opposite to the default forward vector
	else if (glm::abs(dot + 1.0f) < glm::epsilon<float>()) {
		return glm::angleAxis(glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// Calculate the rotation axis
	glm::vec3 rotationAxis = glm::cross(defaultForward, normalizedDirection);
	rotationAxis = glm::normalize(rotationAxis);

	// Calculate the angle
	float angle = glm::acos(dot);

	// Create the quaternion from the angle and axis
	return glm::angleAxis(angle, rotationAxis);
}

glm::quat Utils::RotationFromDirection(const glm::vec3&& direction)
{
	return RotationFromDirection(direction);
}
