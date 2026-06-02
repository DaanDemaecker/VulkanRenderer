include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	1.0.3 #refs/tags/1.0.1
	GIT_SHALLOW true
)

FetchContent_MakeAvailable(glm)


target_link_libraries(VulkanRenderer3D PRIVATE debug glm::glm)

target_link_libraries(RendererOverhaul PRIVATE debug glm::glm)