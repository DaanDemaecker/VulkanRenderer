include(FetchContent)

FetchContent_Declare(
    DDMModelLoader
    GIT_REPOSITORY https://github.com/DaanDemaecker/DDM-ModelLoader.git
    GIT_TAG main
    GIT_SHALLOW true
)

FetchContent_MakeAvailable(DDMModelLoader)

target_link_libraries(VulkanRenderer3D PUBLIC DDMModelLoader)