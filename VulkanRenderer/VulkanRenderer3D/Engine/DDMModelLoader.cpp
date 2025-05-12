// DDMModelLoader

// Header include
#include "DDMModelLoader.h"
#include "Vulkan/Vulkan3D.h"
#include "DataTypes/RenderClasses/Model.h"
#include "Vulkan/Managers/ModelManager.h"
#include "DataTypes/Materials/Material.h"

DDM3::DDMModelLoader::DDMModelLoader()
{
	m_pModelLoader = std::make_unique<DDMML::DDMModelLoader>();
}

void DDM3::DDMModelLoader::LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	std::vector<DDMML::Vertex> ddmVertices{};

	auto mesh = std::make_unique<DDMML::Mesh>();

	m_pModelLoader->LoadModel(filename, mesh.get());
	
	vertices.clear();

	auto model = std::make_unique<DDM3::Model>();
	model->LoadModel(mesh.get());
	DDM3::Vulkan3D::GetInstance().GetModelManager()->AddModel(std::move(model));

}

void DDM3::DDMModelLoader::LoadScene(const std::string& path)
{
	std::vector<std::unique_ptr<DDMML::Mesh>> meshes{};

	m_pModelLoader->LoadScene(path, meshes);

	for (auto& mesh : meshes)
	{
		auto pCurrModel = std::make_unique<DDM3::Model>();
		pCurrModel->LoadModel(mesh.get());
		DDM3::Vulkan3D::GetInstance().GetModelManager()->AddModel(std::move(pCurrModel));
	}

}

void DDM3::DDMModelLoader::ConvertVertices(const std::vector<DDMML::Vertex>& ddmmlVertices, std::vector<Vertex>& vertices)
{

	DDM3::Vertex vertex{};

	vertices.reserve(ddmmlVertices.size());
	for (auto& ddmVertex : ddmmlVertices)
	{
		vertex.color = glm::vec3{ 1,1,1 }; //ddmVertex.color;
		vertex.normal = ddmVertex.normal;
		vertex.pos = ddmVertex.pos;
		vertex.texCoord = ddmVertex.texCoord;
		vertex.tangent = ddmVertex.tangent;

		vertices.emplace_back(vertex);
	}
}
