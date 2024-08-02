add_library(ImGuizmo INTERFACE)

target_include_directories(ImGuizmo INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(VulkanRenderer3D PRIVATE ImGuizmo)
target_link_libraries(VulkanRenderer2D PRIVATE ImGuizmo)