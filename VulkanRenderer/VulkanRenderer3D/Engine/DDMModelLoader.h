// DDMModelLoader.h
// This class will hold the model loader that can be used to load in 3D models of different types

#ifndef _DDM_MODEL_LOADER_
#define _DDM_MODEL_LOADER_

// Parent class include
#include "Singleton.h"

// File includes
#include "DataTypes/Structs.h"
#include "DDMModelLoader/DDMModelLoader.h"

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

		// Uses TinyObjLoader to store a .obj file in a vertex- and indexVector
		// Parameters:
		//     - filename: The name of the obj file
		void LoadModel(const std::string& filename);

		/// <summary>
		/// This function will load in a complete scene and set up the textures
		/// </summary>
		/// <param name="Path: ">Path to the requested scene file</param>
		void LoadScene(const std::string& path);

		/// <summary>
		/// This function converts the vertices from the DDMML::Vertex to the Vertex struct
		/// </summary>
		/// <param name="ddmmlVertices"></param>
		/// <param name="vertices"></param>
		void ConvertVertices(const std::vector<DDMML::Vertex>& ddmmlVertices, std::vector<Vertex>& vertices);

	private:
		std::unique_ptr<DDMML::DDMModelLoader> m_pModelLoader;
	};
}


#endif // !_DDM_MODEL_LOADER_