﻿#version 450 core
out vec4 FragColor;

in struct {
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    vec2 TexCoords;
    vec3 FragPos;
} OutVert;

uniform samplerCube depthMap; // TODO: array for point lights
//uniform samplerCubeArray depthMap;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_ambient0;

uniform vec3 ligthPosition;
uniform int pointLightCount;

uniform float farPlane;

float ShadowCalculation(vec3 fragPos);

void main()
{
    float shadow = ShadowCalculation(OutVert.FragPos);
    vec4 color = 
        (1.0 - shadow) * texture(texture_diffuse0, OutVert.TexCoords) * OutVert.Diffuse + 
        texture(texture_ambient0, OutVert.TexCoords) * OutVert.Ambient + 
        (1 - shadow) * OutVert.Specular;
    color = clamp(color, 0.0, 1.0);
    FragColor = color;
}

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - ligthPosition;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
//    float closestDepth = texture(depthMap, vec4(fragToLight, 0)).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.01; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / farPlane), 1.0);     
    return shadow;
}