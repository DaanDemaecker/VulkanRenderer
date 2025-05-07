// DDMModelLoader

// Header include
#include "DDMModelLoader.h"
#include "Vulkan/Vulkan3D.h"
#include "DataTypes/RenderClasses/Model.h"
#include "Vulkan/Managers/ModelManager.h"
#include "DataTypes/Materials/Material.h"

DDM3::DDMModelLoader::DDMModelLoader()
{
	m_pModelLoader = std::make_unique<DDMML::ModelLoader>();
}

void DDM3::DDMModelLoader::LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	std::vector<DDMML::Vertex> ddmVertices{};

	m_pModelLoader->LoadModel(filename, ddmVertices, indices);
	
	vertices.clear();

	DDM3::Vertex vertex{};

	for (auto& ddmVertex : ddmVertices)
	{
		vertex.color = glm::vec3{ 1,1,1 }; //ddmVertex.color;
		vertex.normal = ddmVertex.normal;
		vertex.pos = ddmVertex.pos;
		vertex.texCoord = ddmVertex.texCoord;
		vertex.tangent = ddmVertex.tangent;

		vertices.push_back(vertex);
	}

}

void DDM3::DDMModelLoader::LoadScene(const std::string& path, std::vector<std::vector<Vertex>>& verticesLists, std::vector<std::vector<uint32_t>>& indicesLists)
{
	std::vector<std::vector<DDMML::Vertex>> ddmVertices{};

	m_pModelLoader->LoadScene(path, ddmVertices, indicesLists);

	verticesLists.clear();
	verticesLists.resize(ddmVertices.size());


	for (int i{}; i<ddmVertices.size(); ++i)
	{
		ConvertVertices(ddmVertices[i], verticesLists[i]);
	}


	auto pModelManager{ DDM3::Vulkan3D::GetInstance().GetModelManager() };


	std::unique_ptr<DDM3::Model> pCurrModel{};
	std::shared_ptr<DDM3::Material> pDefaultMaterial{ std::make_shared<DDM3::Material>() };

	for (int i{}; i < verticesLists.size(); ++i)
	{
		// Load groundplane
		pCurrModel = std::make_unique<DDM3::Model>();

		pCurrModel->LoadModel(verticesLists[i], indicesLists[i]);
		pCurrModel->SetMaterial(pDefaultMaterial);
		pCurrModel->SetRotate(false);
		pCurrModel->SetScale(0.08f, 0.08f, 0.08f);

		pModelManager->AddModel(std::move(pCurrModel));
	}
}

void DDM3::DDMModelLoader::LoadScene(const std::string& path)
{
	std::vector<DDMML::Mesh> meshes{};

	m_pModelLoader->LoadScene(path, meshes);

	std::unique_ptr<DDM3::Model> pCurrModel{};

	for (auto& mesh : meshes)
	{
		pCurrModel = std::make_unique<DDM3::Model>();
		pCurrModel->LoadModel(mesh);
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
