#ifndef ModelIncluded
#define ModelIncluded


namespace D3D
{
	class Material;

	class Model
	{
	public:
		Model();
		~Model();

		Model(Model& other) = delete;
		Model(Model&& other) = delete;

		Model& operator=(Model& other) = delete;
		Model& operator=(Model&& other) = delete;

		void LoadModel(const std::string& textPath);
		void SetMaterial(std::shared_ptr<Material> pMaterial);

		void Update();

		void Render();

		//Getters for rendering
		const VkBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
		const VkBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
		size_t GetIndexAmount() const { return m_Indices.size(); }

		//Transform functions
		void SetPosition(float x, float y, float z);
		void SetRotation(float x, float y, float z);
		void SetScale(float x, float y, float z);

	private:
		//Member variables
		bool m_Initialized{ false };

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

		//Material
		std::shared_ptr<Material> m_pMaterial{};

		//Initialization functions
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateUniformBuffers();
		void CreateDescriptorSets();

		void UpdateDescriptorSets();

		void UpdateUniformBuffer(uint32_t frame);


		//Texture functions
		VkImageView& GetImageView();
		VkSampler& GetSampler();
		PipelinePair& GetPipeline();

		void Cleanup();

		void SetDirtyFlags();
	};
}

#endif // !ModelIncluded