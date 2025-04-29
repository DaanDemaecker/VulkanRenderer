#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 fragColor;
layout(location = 1) in vec3 inColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) in vec2 inTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;

void main()
{
    gl_Position = ((ubo.proj * ubo.view) * ubo.model) * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    mat4 _57 = transpose(inverse(ubo.model));
    fragNormal = mat3(_57[0].xyz, _57[1].xyz, _57[2].xyz) * normal;
}