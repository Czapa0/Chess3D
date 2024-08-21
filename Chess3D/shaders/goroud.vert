#version 410 core

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

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out struct {
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    vec2 TexCoords;
    vec3 FragPos;
} OutVert;

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

Light CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 P);

void main() {
    // vertex part
    OutVert.TexCoords = aTexCoord;
	vec3 worldPos = vec3(modelMatrix * vec4(aPos, 1.0));
    OutVert.FragPos = worldPos;
    vec3 worldNormal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    // fragment part
    Light finalColor;
    finalColor.ambient = finalColor.diffuse = finalColor.specular = vec3(0.0, 0.0, 0.0);
    vec3 V = normalize(cameraPos - worldPos);

    for (int i = 0; i < pointLightCount; i++) {
        Light l = CalcPointLight(pointLights[i], V, aNormal, aPos);
        finalColor.ambient += l.ambient;
        finalColor.diffuse += l.diffuse;
        finalColor.specular += l.specular;
    }

    OutVert.Ambient = vec4(finalColor.ambient, 1.0);
    OutVert.Diffuse = vec4(finalColor.diffuse, 1.0);
    OutVert.Specular = vec4(finalColor.specular, 1.0);
}

Light CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 P) {
    vec3 L = normalize(light.position - P);
    float cosNL = max(dot(N, L), 0.0);
    vec3 R = 2 * dot(N, L) * N - L;
    float cosVRn = max(pow(max(dot(V, R), 0.0), material.shininess), 0.0);

    float distance = length(light.position - P);
    float attenuation =
        1.0 / (light.k0 +
        light.k1 * distance +
        light.k2 * distance * distance);

    Light res;
    res.ambient = light.ambient * material.ambient * attenuation;
    res.diffuse = cosNL * light.diffuse * material.diffuse * attenuation;
    res.specular = cosVRn * light.specular * material.specular * attenuation;
    return res;
}
