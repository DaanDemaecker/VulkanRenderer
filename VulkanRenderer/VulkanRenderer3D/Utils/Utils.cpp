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
