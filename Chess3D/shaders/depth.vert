#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec4 FragPos;

void main()
{
    FragPos = modelMatrix * vec4(aPos, 1.0);
    gl_Position = viewMatrix * FragPos;
}