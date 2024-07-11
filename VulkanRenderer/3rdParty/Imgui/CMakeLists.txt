FetchContent_Populate(
  imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.4.zip
  #GIT_TAG     v1.90.1 # Specify the version you need
  GIT_SHALLOW TRUE
)

add_library(imgui_library STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
 
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui_library PRIVATE ${imgui_SOURCE_DIR})

target_link_libraries(imgui_library PRIVATE glfw)

# Find the required packages
find_package(Vulkan REQUIRED)

target_link_libraries(imgui_library PRIVATE Vulkan::Vulkan)

target_include_directories(VulkanRenderer3D PRIVATE ${imgui_SOURCE_DIR})
target_link_libraries(VulkanRenderer3D PRIVATE imgui_library)

target_include_directories(VulkanRenderer2D PRIVATE ${imgui_SOURCE_DIR})
target_link_libraries(VulkanRenderer2D PRIVATE imgui_library)
