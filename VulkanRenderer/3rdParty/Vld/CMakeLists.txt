add_library(vld STATIC IMPORTED)

set_target_properties(vld PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/Win64/vld.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(VulkanRenderer3D PRIVATE debug vld)

target_link_libraries(RendererOverhaul PRIVATE debug vld)