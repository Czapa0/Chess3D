#version 330 core
out vec4 FragColor;

// TODO: struct for in/out
in vec4 Ambient;
in vec4 Diffuse;
in vec4 Specular;
in vec2 TexCoords;
in vec3 FragPos;

uniform samplerCube depthMap; // TODO: array for point lights
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_ambient0;

uniform vec3 ligthPosition;

uniform float farPlane;

float ShadowCalculation(vec3 fragPos);

void main()
{
    float shadow = ShadowCalculation(FragPos);
    vec4 color = (1.0 - shadow) * texture(texture_diffuse0, TexCoords) * Diffuse + texture(texture_ambient0, TexCoords) * Ambient + (1 - shadow) * Specular;
    color = clamp(color, 0.0, 1.0);
    FragColor = color;
}

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - ligthPosition;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / farPlane), 1.0);     
    return shadow;
}