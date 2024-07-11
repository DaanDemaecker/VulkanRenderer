include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
	GIT_SHALLOW true
)

FetchContent_MakeAvailable(glm)


target_link_libraries(VulkanRenderer3D PRIVATE glm::glm)

target_link_libraries(VulkanRenderer2D PRIVATE glm::glm)