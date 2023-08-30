#ifndef DescriptorPoolManagerIncluded
#define DescriptorPoolManagerIncluded

namespace D3D
{
	class DescriptorPoolManager
	{
	public:
		DescriptorPoolManager() = default;
		~DescriptorPoolManager() = default;

		DescriptorPoolManager(DescriptorPoolManager& other) = delete;
		DescriptorPoolManager(DescriptorPoolManager&& other) = delete;

		DescriptorPoolManager& operator=(DescriptorPoolManager& other) = delete;
		DescriptorPoolManager& operator=(DescriptorPoolManager&& other) = delete;


	private:

	};
}

#endif // !DescriptorPoolManagerIncluded