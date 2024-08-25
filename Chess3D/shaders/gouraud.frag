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

uniform int pointLightCount;

uniform float farPlane;

out vec4 FragColor;

float ShadowCalculation(vec3 fragPos, int light, vec3 position);

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
        (1 - shadow) * PointLight1.specular;
    // 2
    shadow = ShadowCalculation(FragPos, 1, PointLight2.position);
    color += 
        (1.0 - shadow) * texture(texture_diffuse0, TexCoords) * PointLight2.diffuse + 
        texture(texture_ambient0, TexCoords) * PointLight2.ambient + 
        (1 - shadow) * PointLight2.specular;

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