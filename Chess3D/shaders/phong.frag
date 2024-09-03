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

#define MAX_SPOT_LIGHT 5
struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 direction;
    float k0;
    float k1;
    float k2;
    float cone;
    mat4 lightSpaceMatrix;
};
uniform int spotLightCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHT];

uniform vec3 cameraPos;

uniform float farPlane;

uniform bool fogActive;
uniform vec3 fogColor;
uniform float fogIntensity;

uniform samplerCubeArray depthMap;
uniform sampler2DArray depthMapSL;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_ambient0;
uniform sampler2D texture_specular0;

in vec3 Pos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

vec3 CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 P, float shadow);
vec3 CalcSpotLight(SpotLight light, vec3 V, vec3 N, vec3 P, float shadow);
float CalcShadow(vec3 fragPos, int light, vec3 position);
float CalcShadowSL(vec4 fragPosLightSpace, int light, vec3 position);
float CalcFogFactor(vec3 pos);

void main() {
    vec3 finalColor = vec3(0.0, 0.0, 0.0);

    vec3 V = normalize(cameraPos - Pos);

    // point lights
    for(int i = 0; i < pointLightCount; ++i) {
        float shadow = CalcShadow(Pos, i, pointLights[i].position);
        finalColor += CalcPointLight(pointLights[i], V, Normal, Pos, shadow);
    }

    // spot lights
    for(int i = 0; i < spotLightCount; ++i) {
        vec4 fragPosLightSpace = spotLights[i].lightSpaceMatrix * vec4(Pos, 1.0);
        float shadow = CalcShadowSL(fragPosLightSpace, i, spotLights[i].position);
        finalColor += CalcSpotLight(spotLights[i], V, Normal, Pos, shadow);
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
    return (1.0 - shadow) * (specular + diffuse) + ambient;
}

vec3 CalcSpotLight(SpotLight light, vec3 V, vec3 N, vec3 P, float shadow) {
    vec3 L = normalize(light.position - P);
    float cosNL = max(dot(N, L), 0.0);
    vec3 R = 2 * dot(N, L) * N - L;
    float cosVRn = pow(max(dot(V, R), 0.0), material.shininess);
    float cosDLc = pow(max(dot(normalize(-light.direction), L), 0.0), light.cone);

    float distance = length(light.position - P);
    float attenuation =
        1.0 / (light.k0 +
        light.k1 * distance +
        light.k2 * distance * distance);

    vec3 ambient = light.ambient * material.ambient * vec3(texture(texture_ambient0, TexCoords)) * attenuation * cosDLc;
    vec3 diffuse = cosNL * light.diffuse * material.diffuse * vec3(texture(texture_diffuse0, TexCoords)) * attenuation * cosDLc;
    vec3 specular = cosVRn * light.specular * material.specular * vec3(texture(texture_specular0, TexCoords)) * attenuation * cosDLc;
    return ambient + (1.0f - shadow) * (diffuse + specular);
}

float CalcShadow(vec3 fragPos, int light, vec3 position)
{
    vec3 fragToLight = fragPos - position;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias   = 0.03;
    int samples  = 20;
    float viewDistance = length(cameraPos - Pos);
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

float CalcShadowSL(vec4 fragPosLightSpace, int light, vec3 position){
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMapSL, vec3(projCoords.xy, light)).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 lightDir = normalize(position - Pos);
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
//    // PCF
//    float shadow = 0.0;
//    vec2 texelSize = 1.0 / vec2(textureSize(depthMapSL, 0)); // ???
//    for(int x = -1; x <= 1; ++x)
//    {
//        for(int y = -1; y <= 1; ++y)
//        {
//            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
//            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
//        }    
//    }
//    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
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