#version 450

layout(binding = 1) uniform UniformLightObject
{
    vec3 direction;
    vec3 color;
    float intensity;
} light;

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 fragColor;
layout(location = 2) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

float GetObservedArea(vec3 normal)
{
    float dotProduct = dot(normal, -light.direction);
    float observedArea = clamp(dotProduct, 0.0, 1.0);
    return observedArea;
}

void main()
{
    vec3 param = fragNormal;
    outColor = vec4(((fragColor * GetObservedArea(param)) * light.color) * light.intensity, 1.0);
}