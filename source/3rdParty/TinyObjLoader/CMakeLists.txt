include(FetchContent)

FetchContent_Declare(
	tinyOBJLoader
	GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
	GIT_TAG v1.0.6
    GIT_SHALLOW true
)

# Make the fetched content available
FetchContent_MakeAvailable(tinyOBJLoader)

# Link libraries
target_link_libraries(VulkanRenderer3D PRIVATE tinyobjloader)