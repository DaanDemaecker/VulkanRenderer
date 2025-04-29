#version 450

layout(binding = 0) uniform LightSpaceMatrix {
    mat4 matrix;
} lightSpaceMatrix;

layout(push_constant) uniform PushConstants {
    mat4 modelTransform;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;

void main() {
    gl_Position = lightSpaceMatrix.matrix * pushConstants.modelTransform * vec4(inPosition, 1.0);
}
