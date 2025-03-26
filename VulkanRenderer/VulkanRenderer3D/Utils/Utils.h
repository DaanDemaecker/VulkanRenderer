// Utils.h
// This file defines utility functions with the Utils namespace
// Currently these functions include reading in binary files and obj files

#ifndef UtilsIncluded
#define UtilsIncluded

// File includes
#include "DataTypes/Structs.h"

// Standard library includes
#include <vector>
#include <fstream>

namespace Utils
{
	// Reads in a file and returns it as a char vector
	// This function is mainly used to read in shaders
	// Parameters:
	//     - filename: The name of the file to be read
	std::vector<char> readFile(const std::string& filename);

	std::string GetExtension(const std::string& filename);

	glm::quat RotationFromDirection(const glm::vec3& direction);

	glm::quat RotationFromDirection(const glm::vec3&& direction);
}

#endif // !UtilsIncluded