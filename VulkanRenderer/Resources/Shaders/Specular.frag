#version 450

layout(binding = 1) uniform UniformLightObject {
    vec3 direction;
    vec3 color;
    float intensity;
} light;

layout(binding = 2) uniform sampler2D glossMap;
layout(binding = 3) uniform sampler2D specMap;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragTangent;
layout(location = 4) in vec3 cameraPosition;
layout(location = 5) in vec3 worldPosition;

layout(location = 0) out vec4 outColor;

float GetObservedArea(vec3 normal);

vec4 CalculateSpecular(vec3 normal, vec3 viewDirection);

void main()
{
    

    float observedArea = GetObservedArea(fragNormal);

    vec4 specular = CalculateSpecular(fragNormal, normalize(worldPosition - cameraPosition));
    
    
    vec3 finalColor = (fragColor + specular.xyz) * observedArea;
    outColor = vec4(finalColor, 1);
}

float GetObservedArea(vec3 normal)
{
    float dotProduct = dot(normal, -light.direction);
    float observedArea = clamp(dotProduct, 0, 1);
    return observedArea;
}

vec4 CalculateSpecular(vec3 normal, vec3 viewDirection)
{
	vec3 reflected = reflect(light.direction, normal);

	float cosAngle = clamp(dot(reflected, viewDirection), 0, 1);

	float exp = texture(glossMap, fragTexCoord).r;

	float phongSpecular = pow(cosAngle, exp);

	return texture(specMap, fragTexCoord) * phongSpecular;
}