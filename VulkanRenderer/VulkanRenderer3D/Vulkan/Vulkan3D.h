// Vulkan3D.h

#ifndef Vulkan3DIncluded
#define Vulkan3DIncluded

// File includes
#include "Engine/Singleton.h"
#include "Includes/VulkanIncludes.h"
#include "Vulkan/Renderers/VulkanRenderer3D.h"
#include "DataTypes/Camera.h"

// Standard library includes
#include <memory>

namespace D3D
{
	// Class forward declarations
	class DispatchableManager;
	class ModelManager;
	class Model;
	class GPUObject;
	class CameraManager;

	class Vulkan3D final : public Singleton<Vulkan3D>
	{
	public:
		// Destructor
		~Vulkan3D();
		
		// Delete copy and move functions
		Vulkan3D(Vulkan3D& other) = delete;
		Vulkan3D(Vulkan3D&& other) = delete;
		Vulkan3D& operator=(Vulkan3D& other) = delete;
		Vulkan3D& operator=(Vulkan3D && other) = delete;

		static uint32_t GetMaxFrames() { return m_sMaxFramesInFlight; }
		static uint32_t GetCurrentFrame() { return m_sCurrentFrame; }

		// Initialize the renderer, must be called at start of program
		void Init();

		// Terminate the renderer, must be called at end of program
		void Terminate();

		// Get vulkan instance
		VkInstance GetVulkanInstance() const;

		// Get logical device
		VkDevice GetDevice() const;

		// Get physical device
		VkPhysicalDevice GetPhysicalDevice() const;

		// Get GPU object
		GPUObject* GetGPUObject() const;

		// Get surface
		VkSurfaceKHR GetSurface() const;

		VulkanRenderer3D& GetRenderer();

		// Main render function
		void Render();

		// Get model manager
		ModelManager* GetModelManager();
	
		void SetCamera(std::unique_ptr<Camera> pNewCamera);

		Camera* GetCurrentCamera();

	private:
		// Private  default constructor accessable for singleton class
		friend class Singleton<Vulkan3D>;
		Vulkan3D();

		// The maximum amount of frames in flight
		static uint32_t m_sMaxFramesInFlight;

		// The current frame
		static uint32_t m_sCurrentFrame;

		// Dispatchable manager
		std::unique_ptr<D3D::DispatchableManager> m_pDispatchableManager{};

		// Renderer
		std::unique_ptr<D3D::VulkanRenderer3D> m_pRenderer{};

		// Model Manager
		std::unique_ptr<D3D::ModelManager> m_pModelManager{};

		// Camera Manager
		std::unique_ptr<D3D::CameraManager> m_pCameraManager{};
	};

}
#endif // !Vulkan3DIncluded