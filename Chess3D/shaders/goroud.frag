#version 330 core
out vec4 FragColor;

in vec4 Ambient;
in vec4 Diffuse;
in vec4 Specular;
in vec2 TexCoords;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_ambient0;

void main()
{
    vec4 color = texture(texture_diffuse0, TexCoords) * Diffuse + texture(texture_ambient0, TexCoords) * Ambient + Specular;
    color = clamp(color, 0.0, 1.0);
    FragColor = color;
}