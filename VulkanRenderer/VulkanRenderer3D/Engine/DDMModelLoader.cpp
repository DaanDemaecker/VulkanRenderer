// DDMModelLoader

// Header include
#include "DDMModelLoader.h"

DDM3::DDMModelLoader::DDMModelLoader()
{
	m_pModelLoader = std::make_unique<DDM::ModelLoader>();
}

void DDM3::DDMModelLoader::LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	std::vector<DDM::Vertex> ddmVertices{};

	m_pModelLoader->LoadModel(filename, ddmVertices, indices);
	
	vertices.clear();

	DDM3::Vertex vertex{};

	for (auto& ddmVertex : ddmVertices)
	{
		vertex.color = ddmVertex.color;
		vertex.normal = ddmVertex.normal;
		vertex.pos = ddmVertex.pos;
		vertex.texCoord = ddmVertex.texCoord;
		vertex.tangent = ddmVertex.tangent;

		vertices.push_back(vertex);
	}

}
