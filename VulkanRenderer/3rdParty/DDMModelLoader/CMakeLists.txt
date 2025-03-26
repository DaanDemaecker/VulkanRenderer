include(FetchContent)

FetchContent_Declare(
    DDMModelLoader
    GIT_REPOSITORY https://github.com/DaanDemaecker/DDM-ModelLoader.git
    GIT_TAG main
    GIT_SHALLOW true
)

FetchContent_MakeAvailable(DDMModelLoader)

# Link against DDMModelLoader
target_link_libraries(VulkanRenderer3D PRIVATE DDMModelLoader)

target_include_directories(VulkanRenderer3D PUBLIC 
    ${DDMModelLoader_SOURCE_DIR} 
    ${DDMModelLoader_BINARY_DIR}
)



add_custom_target(CopyFbxSdkDll ALL)
add_custom_command(
    TARGET CopyFbxSdkDll POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${DDMModelLoader_BINARY_DIR}/libfbxsdk.dll
    $<TARGET_FILE_DIR:VulkanRenderer3D>
)
