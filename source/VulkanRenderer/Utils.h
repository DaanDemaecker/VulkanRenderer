// Utils.h
// This file defines utility functions with the Utils namespace
// Currently these functions include reading in binary files and obj files

#ifndef UtilsIncluded
#define UtilsIncluded

// File includes
#include "Structs.h"

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

	// Uses TinyObjLoader to store a .obj file in a vertex- and indexVector
	// Parameters:
	//     - filename: The name of the obj file
	//     - vertices: The vector that will be used to store the vertices
	//     - indices: The vector that will be used to store the indices
	void LoadModel(const std::string& filename, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);
}

#endif // !UtilsIncluded