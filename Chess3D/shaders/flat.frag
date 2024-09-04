#version 450 core

struct Light{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
    vec4 fragPosLightSpace;
};

in Light PointLight1;
in Light PointLight2;
in Light SpotLight1;
in Light SpotLight2;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 FragNormal;

uniform samplerCubeArray depthMap;
uniform sampler2DArray depthMapSL;
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

int diskSize = 16;
vec2 poissonDisk[] = vec2[](
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float ShadowCalculation(vec3 fragPos, int light, vec3 position);
float CalcShadowSL(vec4 fragPosLightSpace, int light, vec3 position);
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

    // spot lights
    // 1.
    shadow = CalcShadowSL(SpotLight1.fragPosLightSpace, 0, SpotLight1.position);
    color += 
        (1.0 - shadow) * texture(texture_diffuse0, TexCoords) * SpotLight1.diffuse + 
        texture(texture_ambient0, TexCoords) * SpotLight1.ambient + 
        (1 - shadow) * texture(texture_specular0, TexCoords) *  SpotLight1.specular;
    // 2.
    shadow = CalcShadowSL(SpotLight2.fragPosLightSpace, 1, SpotLight2.position);
    color += 
        (1.0 - shadow) * texture(texture_diffuse0, TexCoords) * SpotLight2.diffuse + 
        texture(texture_ambient0, TexCoords) * SpotLight2.ambient + 
        (1 - shadow) * texture(texture_specular0, TexCoords) *  SpotLight2.specular;

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
    float bias   = max(0.1 * (1.0 - dot(FragNormal, -normalize(fragToLight))), 0.05);
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
    vec3 lightDir = normalize(position - FragPos);
    float bias = 0.0f; // TODO: probably to be removed
    // PCF
    float shadow = 0.0;
    float spreadInv = 1.0 / 700.0;
    for (int i = 0; i < diskSize; ++i) {
       float pcfDepth = texture(depthMapSL, vec3(projCoords.xy + poissonDisk[i] * spreadInv, light)).r; 
       shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
    }
    shadow /= diskSize;
    
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