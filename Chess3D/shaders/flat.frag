#version 450 core

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

flat in struct {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
} PointLight1;
flat in struct {
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
        (1 - shadow) * texture(texture_specular0, TexCoords) *  PointLight2.specular;

    // fog
    float fog = fogActive ? CalcFogFactor(FragPos) : 1.0;
    color = mix(vec4(fogColor, 1.0), color, fog);

    color = clamp(color, 0.0, 1.0);
    FragColor = color;
}

float ShadowCalculation(vec3 fragPos, int light, vec3 position)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - position;
    // use the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, vec4(fragToLight, light)).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.03; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
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