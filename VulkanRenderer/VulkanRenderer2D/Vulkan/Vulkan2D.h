// Vulkan2D 
// This class will hold, managa and handle all Vulkan objects

#ifndef Vulkan2DIncluded

// File includes
#include "Engine/Singleton.h"
#include "DispatchableManager.h"

// Standard library includes
#include <memory>

namespace DDM2
{
	class Vulkan2D final : public Singleton<Vulkan2D>
	{
	public:
		~Vulkan2D();

		void Init();

		void Terminate();

		DispatchableManager* GetDispatchableManager() const;

	private:
		friend class Singleton<Vulkan2D>;
		Vulkan2D();

		std::unique_ptr<DispatchableManager> m_pDispatchableManager{};
	};
}

#endif // !Vulkan2DIncluded

