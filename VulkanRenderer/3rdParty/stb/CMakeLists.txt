add_library(stb INTERFACE IMPORTED)

set_target_properties(stb PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(VulkanRenderer3D PRIVATE general stb)

target_link_libraries(RendererOverhaul PRIVATE general stb)