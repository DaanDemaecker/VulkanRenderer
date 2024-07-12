#version 450

layout(binding = 2) uniform UniformLightObject {
    vec3 direction;
    vec3 color;
    float intensity;
} light;

layout(binding = 3) uniform sampler2D texSampler;

layout(binding = 4) uniform sampler2D shadowSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec4 lightPos;

layout(location = 0) out vec4 outColor;

float GetObservedArea(vec3 normal);

float CalculateShadowAmount();

float shadowConstant = 0.8f;
float tolerance = 0.0005f;

void main()
{
	float observedArea = GetObservedArea(normalize(fragNormal));

    vec4 sampledColor =texture(texSampler, fragTexCoord);

	vec3 finalColor = sampledColor.rgb;
	
	finalColor *= light.color * light.intensity * observedArea * CalculateShadowAmount();

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

float CalculateShadowAmount()
{
    //re-homogenize position after interpolation
    vec3 lpos = lightPos.xyz / lightPos.w;
 
    //if position is not visible to the light - dont illuminate it
    //results in hard light frustum
    if( lpos.x < -1.0f || lpos.x > 1.0f ||
        lpos.y < -1.0f || lpos.y > 1.0f ||
        lpos.z < 0.0f  || lpos.z > 1.0f ) return 1;
 
    //transform clip space coords to texture space coords (-1:1 to 0:1)
    lpos.x = lpos.x/2 + 0.5;
    lpos.y = lpos.y/2 + 0.5;
 
    //sample shadow map - point sampler
    float shadowMapDepth = texture(shadowSampler, lpos.xy).r;
 
    //if clip space z value greater than shadow map value then pixel is in shadow
    if ( shadowMapDepth < lpos.z - tolerance) return shadowConstant;
 
    return 1;
}