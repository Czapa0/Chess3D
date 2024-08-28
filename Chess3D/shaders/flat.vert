#version 450 core

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTriPos;
layout(location = 4) in vec3 aFaceNormal;

out struct {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
} PointLight1;
out struct {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
} PointLight2;
out vec2 TexCoords;
out vec3 FragPos;

Light CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 P);

void main() {
    // vertex part
    TexCoords = aTexCoord;
	vec3 worldPos = vec3(modelMatrix * vec4(aTriPos, 1.0));
    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));;
    vec3 worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * aFaceNormal);
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    // fragment part
    vec3 V = normalize(cameraPos - worldPos);

    Light l1 = CalcPointLight(pointLights[0], V, worldNormal, worldPos);
    PointLight1.ambient = vec4(l1.ambient, 1.0);
    PointLight1.diffuse = vec4(l1.diffuse, 1.0);
    PointLight1.specular = vec4(l1.specular, 1.0);
    PointLight1.position = pointLights[0].position;

    Light l2 = CalcPointLight(pointLights[1], V, worldNormal, worldPos);
    PointLight2.ambient = vec4(l2.ambient, 1.0);
    PointLight2.diffuse = vec4(l2.diffuse, 1.0);
    PointLight2.specular = vec4(l2.specular, 1.0);
    PointLight2.position = pointLights[1].position;
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
