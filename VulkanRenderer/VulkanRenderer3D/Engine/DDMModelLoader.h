// DDMModelLoader.h
// This class will hold the model loader that can be used to load in 3D models of different types

#ifndef _DDM_MODEL_LOADER_
#define _DDM_MODEL_LOADER_

// Parent class include
#include "Singleton.h"

// File includes
#include "DataTypes/Structs.h"
#include "src/DDMModelLoader.h"

// Standard library includes
#include <memory>
#include <vector>
#include <string>

namespace DDM3
{
	class DDMModelLoader final : public Singleton<DDMModelLoader>
	{
	public:
		DDMModelLoader();

		// Uses TinyObjLoader to store a .obj file in a vertex- and indexVector
	// Parameters:
	//     - filename: The name of the obj file
	//     - vertices: The vector that will be used to store the vertices
	//     - indices: The vector that will be used to store the indices
		void LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

	private:
		std::unique_ptr<DDM::ModelLoader> m_pModelLoader;
	};
}


#endif // !_DDM_MODEL_LOADER_