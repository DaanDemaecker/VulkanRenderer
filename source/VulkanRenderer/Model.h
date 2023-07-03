#pragma once

namespace D3D
{
	class Model
	{
	public:
		Model();
		~Model();

		Model(Model& other) = delete;
		Model(Model&& other) = delete;

		Model& operator=(Model& other) = delete;
		Model& operator=(Model&& other) = delete;

	private:
		//Member variables
		std::vector<Vertex> m_Vertices{};
		std::vector<uint32_t> m_Indices{};

		//Vertex buffer
		VkBuffer m_VertexBuffer{};
		VkDeviceMemory m_VertexBufferMemory{};

		//Index buffer
		VkBuffer m_IndexBuffer{};
		VkDeviceMemory m_IndexBufferMemory{};


		//Initialization functions
		void CreateVertexBuffer();
		void CreateIndexBuffer();
	};
}

