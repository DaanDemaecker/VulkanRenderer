add_library(stb INTERFACE)

target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(VulkanRenderer3D PRIVATE stb)

target_link_libraries(VulkanRenderer2D PRIVATE stb)