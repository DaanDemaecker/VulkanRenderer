#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tanget;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) out vec3 outUVW;

void main()
{
    outUVW = inPosition;
    mat4 viewMat = mat4(mat3(ubo.view));
    gl_Position = ubo.proj * viewMat * vec4(inPosition.xyz, 1.0);
}