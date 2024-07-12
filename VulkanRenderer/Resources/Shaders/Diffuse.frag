#version 450

layout(binding = 1) uniform UniformLightObject {
    vec3 direction;
    vec3 color;
    float intensity;
} light;

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

float GetObservedArea(vec3 normal);

void main()
{
	float observedArea = GetObservedArea(normalize(fragNormal));

    vec4 sampledColor =texture(texSampler, fragTexCoord);

	vec3 finalColor = sampledColor.rgb;
	
	finalColor *= light.color * light.intensity * observedArea;

	outColor = vec4(finalColor, sampledColor.w);

    float alphaThreshold = 0.1;
    if(outColor.w < alphaThreshold)
    {
        discard;
    } 
}

float GetObservedArea(vec3 normal)
{
	float dotProduct = dot(normal, -light.direction);
    float observedArea = clamp(dotProduct, 0, 1);
    return observedArea;
}