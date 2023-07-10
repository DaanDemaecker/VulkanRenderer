#ifndef ModelIncluded
#define ModelIncluded


namespace D3D
{
	class Model
	{
	public:
		Model(bool normal = true);
		~Model();

		Model(Model& other) = delete;
		Model(Model&& other) = delete;

		Model& operator=(Model& other) = delete;
		Model& operator=(Model&& other) = delete;

		void Render(VkCommandBuffer& commandBuffer, uint32_t frame);

		//Getters for rendering
		const VkBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
		const VkBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
		size_t GetIndexAmount() const { return m_Indices.size(); }

		//Transform functions
		void SetPosition(float x, float y, float z) { m_Position = { x, y, z }; }
		void SetRotation(float x, float y, float z) { m_Rotation = { x, y, z }; }
		void SetScale(float x, float y, float z) { m_Scale = { x, y, z }; }

	private:
		//Member variables
		glm::vec3 m_Rotation{};
		glm::vec3 m_Position{};
		glm::vec3 m_Scale{ 1, 1, 1 };

		std::vector<UniformBufferObject> m_Ubos{};
		std::vector<bool> m_UboChanged{};

		std::vector<VkBuffer> m_UboBuffers{};
		std::vector<VkDeviceMemory> m_UbosMemory{};
		std::vector<void*> m_UbosMapped{};

		std::vector<VkDescriptorSet> m_DescriptorSets{};


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
		void CreateUniformBuffers();
		void CreateDescriptorSets();

		void UpdateUniformBuffer(uint32_t frame);
	};
}

#endif // !ModelIncluded