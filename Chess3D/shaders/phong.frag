#version 450 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

#define MAX_POINT_LIGHT 5
struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    float k0;
    float k1;
    float k2;
};
uniform int pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHT];

uniform vec3 cameraPos;

uniform float farPlane;

uniform bool fogActive;
uniform vec3 fogColor;
uniform float fogIntensity;

uniform samplerCubeArray depthMap;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_ambient0;
uniform sampler2D texture_specular0;

in vec3 Pos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

vec3 CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 P, float shadow);
float CalcShadow(vec3 fragPos, int light, vec3 position);
float CalcFogFactor(vec3 pos);

void main() {
    vec3 finalColor = vec3(0.0, 0.0, 0.0);

    vec3 V = normalize(cameraPos - Pos);

    // point lights
    for(int i = 0; i < pointLightCount; ++i) {
        float shadow = CalcShadow(Pos, i, pointLights[i].position);
        finalColor += CalcPointLight(pointLights[i], V, Normal, Pos, shadow);
    }

    // fog
    float fog = fogActive ? CalcFogFactor(Pos) : 1.0;
    finalColor = mix(fogColor, finalColor, fog);

    FragColor = clamp(vec4(finalColor, 1.0), 0, 1);
}

vec3 CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 P, float shadow) {
    vec3 L = normalize(light.position - P);
    float cosNL = max(dot(N, L), 0.0);
    vec3 R = 2 * dot(N, L) * N - L;
    float cosVRn = max(pow(max(dot(V, R), 0.0), material.shininess), 0.0);

    float distance = length(light.position - P);
    float attenuation =
        1.0 / (light.k0 +
        light.k1 * distance +
        light.k2 * distance * distance);

    vec3 ambient = light.ambient * material.ambient * vec3(texture(texture_ambient0, TexCoords)) * attenuation;
    vec3 diffuse = cosNL * light.diffuse * material.diffuse * vec3(texture(texture_diffuse0, TexCoords)) * attenuation;
    vec3 specular = cosVRn * light.specular * material.specular * vec3(texture(texture_specular0, TexCoords)) * attenuation;
//    return specular;
    return (1.0 - shadow) * (specular + diffuse) + ambient;
}

float CalcShadow(vec3 fragPos, int light, vec3 position)
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