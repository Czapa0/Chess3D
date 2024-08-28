#version 450 core

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in struct {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
} PointLight1;
in struct {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
} PointLight2;
in vec2 TexCoords;
in vec3 FragPos;

uniform samplerCubeArray depthMap;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_ambient0;
uniform sampler2D texture_specular0;

uniform vec3 cameraPos;

uniform int pointLightCount;
uniform float farPlane;

uniform bool fogActive;
uniform float fogIntensity;
uniform vec3 fogColor;

out vec4 FragColor;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float ShadowCalculation(vec3 fragPos, int light, vec3 position);
float CalcFogFactor(vec3 pos);

void main()
{
    float shadow;
    vec4 color;

    // point lights
    // 1
    shadow = ShadowCalculation(FragPos, 0, PointLight1.position);
    color = 
        (1.0 - shadow) * texture(texture_diffuse0, TexCoords) * PointLight1.diffuse + 
        texture(texture_ambient0, TexCoords) * PointLight1.ambient + 
        (1 - shadow) * texture(texture_specular0, TexCoords) * PointLight1.specular;
    // 2
    shadow = ShadowCalculation(FragPos, 1, PointLight2.position);
    color += 
        (1.0 - shadow) * texture(texture_diffuse0, TexCoords) * PointLight2.diffuse + 
        texture(texture_ambient0, TexCoords) * PointLight2.ambient + 
        (1 - shadow) * texture(texture_specular0, TexCoords) * PointLight2.specular;

    // fog
    float fog = fogActive ? CalcFogFactor(FragPos) : 1.0;
    color = mix(vec4(fogColor, 1.0), color, fog);

    color = clamp(color, 0.0, 1.0);
    FragColor = color;
}

float ShadowCalculation(vec3 fragPos, int light, vec3 position)
{
    vec3 fragToLight = fragPos - position;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias   = 0.03;
    int samples  = 20;
    float viewDistance = length(cameraPos - FragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 100.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, light)).r;
        closestDepth *= farPlane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}

float CalcFogFactor(vec3 pos)
{
    if (fogIntensity == 0) return 1;
    float gradient = (fogIntensity * fogIntensity - 50 * fogIntensity + 60) / 5;
    float dist = length(cameraPos - pos);
    float fog = exp(-pow((dist / gradient), 4));
    fog = clamp(fog, 0.0, 1.0);
    return fog;
}