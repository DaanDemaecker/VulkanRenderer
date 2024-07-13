# Set project
project(VulkanRenderer3D)

# Include directories
include_directories(${PROJECT_SOURCE_DIR})

# Create the executable
add_executable(VulkanRenderer3D
	"DataTypes/DescriptorObjects/TextureDescriptorObject.cpp"
	"DataTypes/Materials/CubeMapMaterial.cpp"
	"DataTypes/Materials/Material.cpp"
	"DataTypes/Materials/ShadowMaterial.cpp"
	"DataTypes/Materials/TexturedMaterial.cpp"
	"DataTypes/RenderClasses/Mesh.cpp"
	"DataTypes/RenderClasses/Model.cpp"
	"DataTypes/RenderClasses/SkyBox.cpp"
	"DataTypes/Camera.cpp"
	"DataTypes/DirectionalLightObject.cpp"
	"Engine/ConfigManager.cpp"
	"Engine/D3DEngine.cpp"
	"Engine/main.cpp"
	"Engine/TimeManager.cpp"
	"Engine/Window.cpp"
	"Includes/STBIncludes.cpp"
	"Includes/TinyObjLoaderIncludes.cpp"
	"Utils/Utils.cpp"
	"Vulkan/Managers/BufferManager.cpp"
	"Vulkan/Managers/CommandpoolManager.cpp"
	"Vulkan/Managers/ImageManager.cpp"
	"Vulkan/Managers/ImageViewManager.cpp"
	"Vulkan/Managers/PipelineManager.cpp"
	"Vulkan/Managers/SyncObjectManager.cpp"
	"Vulkan/Renderers/ShadowRenderer.cpp"
	"Vulkan/Renderers/VulkanRenderer3D.cpp"
	"Vulkan/SpirVReflect/spirv_reflect.cpp"
	"Vulkan/Wrappers/DescriptorPoolWrapper.cpp"
	"Vulkan/Wrappers/GPUObject.cpp"
	"Vulkan/Wrappers/ImGuiWrapper.cpp"
	"Vulkan/Wrappers/InstanceWrapper.cpp"
	"Vulkan/Wrappers/PipelineWrapper.cpp"
	"Vulkan/Wrappers/RenderpassWrapper.cpp"
	"Vulkan/Wrappers/ShaderModuleWrapper.cpp"
	"Vulkan/Wrappers/SurfaceWrapper.cpp"
	"Vulkan/Wrappers/SwapchainWrapper.cpp"
	"Vulkan/Wrappers/Viewport.cpp"
	"Vulkan/VulkanUtils.cpp")

add_custom_target(configFile3D ALL)
add_custom_command(
	TARGET configFile3D POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	-E copy
	${CMAKE_CURRENT_SOURCE_DIR}/Config.json
	$<TARGET_FILE_DIR:VulkanRenderer3D>
)

# Copy resources after Shaders
add_custom_target(copy_resources_3D ALL)
add_custom_command(
    TARGET copy_resources_3D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:VulkanRenderer3D>/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:VulkanRenderer3D>/resources
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/resources
    DEPENDS Shaders
)