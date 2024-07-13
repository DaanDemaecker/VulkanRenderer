// ImageManager.h
// This class will handle everything having to do with images

#ifndef ImageManagerIncluded
#define ImageManagerIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <iostream>

namespace D3D
{
	// Class forward declarations
	class GPUObject;
	class BufferManager;
	class CommandpoolManager;


	class ImageManager final
	{
	public:
		// Constructor
		// Parameter:
		//     pGPUObject : a pointer to the GPU object
		//     pBufferManager: a poienter to the buffer manager
		//     pCommandPoolManager: a pointer to the commandpool manager
		ImageManager(GPUObject* pGPUObject, D3D::BufferManager* pBufferManager, CommandpoolManager* pCommandPoolManager);
		
		// Delete default constructor
		ImageManager() = delete;

		// Default destructor
		~ImageManager() = default;

		// Delete move and copy functions
		ImageManager(ImageManager& other) = delete;
		ImageManager(ImageManager&& other) = delete;
		ImageManager& operator=(ImageManager& other) = delete;
		ImageManager& operator=(ImageManager&& other) = delete;

		// Function for deleting alocated objects
		// Parameters:
		//     device: handle of the VkDevice
		void Cleanup(VkDevice device);

		// Get the default image view
		VkImageView& GetDefaultImageView() { return m_DefaultTexture.imageView; }

		// Get the standard image sampler
		VkSampler& GetSampler() { return m_TextureSampler; }

		// Function for copying a buffer to an image
		// Parameters:
		//     commandBuffer: the single time command buffer needed for the copying
		//     buffer: the buffer that needs to be copied
		//     image: the destination image
		//     width: the width of the image
		//     height: the height of the image
		void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount = 1);

		// Generate mipmaps of a single image
		// Parameters:
		//     physicalDevice: handle of the VkPhysicalDevice
		//     commandBuffer: the commandbuffer needed for the mipmap generation
		//     image: the source image
		//     imageFormat: the format the image is in
		//     texWidth: the width of the image
		//     texHeight: the height of the image
		//     mipLevels: the amount of mipmaps that will be generated
		void GenerateMipmaps(VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer,
			VkImage image, VkFormat imageFormat,
			int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		// Create a given texture image
		// Parameters:
		//     pGPUObject : a pointer to the GPU object 
		//     pBufferManager: a poitner to the buffer manager object
		//     texture: reference to the texture that will be created
		//     textureName: filepath to the texture
		//     pCommandPoolManager: pointer to the commandpool manager
		void CreateTextureImage(GPUObject* pGPUObject, D3D::BufferManager* pBufferManager, Texture& texture,
			const std::string& textureName, CommandpoolManager* pCommandPoolManager);

		// Create a given cube texture image
		// Parameters:
		//     pGPUObject : a pointer to the GPU object 
		//     pBufferManager: pointer to the Buffermanager object
		//     cubeTexture: reference to the texture that will be created
		//     textureNames: filepaths to the texture that make up the faces of the cube
		//     pCommandPoolManager: pointer to the commandpool manager
		void CreateCubeTexture(GPUObject* pGPUObject, D3D::BufferManager* pBufferManager, Texture& cubeTexture,
			const std::initializer_list<const std::string>& textureNames, CommandpoolManager* pCommandPoolManager);

		// Create a texture sampler
		// Parameters:
		//     pGPUObject : a pointer to the GPU object 
		//     sampler: a reference to the sampler that will be created
		//     samples: the max amount of samples
		void CreateTextureSampler(GPUObject* pGPUObject, VkSampler& sampler, uint32_t samples);

		// Create and return an image view
		// Parameters:
		//     device: handle to the VkDevice:
		//     image: the image for the image view
		//     format: the format the image is in
		//     aspectFlags: the flags for the aspect maskµ
		//     mipLevels: the amount of mipmaps that will be generated
		VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		
		// Transition an image from one layout to another
		// Parameters:
		//     image: the image that will be transitioned
		//     commandBuffer: a command buffer needed for the transition
		//     format: the format the image is in
		//     oldLayout: the current layout of the image
		//     newLayout: the desired layout of the image
		//     mipLevels: the amount of mipmaps in the image
		//     layerCount: the amount of images stored in the single VkImage object, 1 by default
		void TransitionImageLayout(VkImage image, VkCommandBuffer commandBuffer, VkFormat format,
			VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount = 1);


		// Create an image
		// Parameters:
		//     pGPUObject : a pointer to the GPU object 
		//     width: the width of the image
		//     height: the height of the image
		//     mipLevels: the amount of mipmaps
		//     numsaples: the amount of samples for multisampling
		//     format: the format the image will be in
		//     tiling: the type of image tiling that will be used
		//     usage: the usage for the image
		//     properties: the requested properties for the image
		//     texture: reference to the texture to be created
		void CreateImage(GPUObject* pGPUObject,
			uint32_t width, uint32_t height, uint32_t mipLevels,
			VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, Texture& texture);

	private:
		// The default texture
		Texture m_DefaultTexture{};

		// The textpath to the default texture
		const std::string m_DefaultTextureName;
		
		// The default sampler
		VkSampler m_TextureSampler{};

		// Check if a requested format has the stencil component
		// Parameters:
		//     format: the format to be checked
		bool HasStencilComponent(VkFormat format);

		// Create the default textures
		// Parameters:
		//     pGPUObject: a pointer to the GPU object 
		//     pBufferManager: a pointer to the buffer manager
		//     pCommandPoolManager: a pointer to the commandpool manager
		void CreateDefaultResources(GPUObject* pGPUObject, D3D::BufferManager* pBufferManager, CommandpoolManager* pCommandPoolManager);
	};
}
#endif // !ImageManagerIncluded