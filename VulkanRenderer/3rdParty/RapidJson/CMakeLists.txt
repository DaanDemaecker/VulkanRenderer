add_library(rapidjson INTERFACE IMPORTED)

set_target_properties(rapidjson PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(VulkanRenderer3D PRIVATE general rapidjson)
target_link_libraries(RendererOverhaul PRIVATE general rapidjson)