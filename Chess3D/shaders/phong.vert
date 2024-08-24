#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 Pos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
    Pos = vec3(modelMatrix * vec4(aPos, 1.0));;
    Normal = normalize(mat3(transpose(inverse(modelMatrix))) * aNormal);
    TexCoords = aTexCoord;
}