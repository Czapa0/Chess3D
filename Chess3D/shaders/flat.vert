#version 410 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_ambient0;

#define MAX_DIR_LIGHT 5
struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};
uniform int dirLightCount;
uniform DirLight dirLights[MAX_DIR_LIGHT];

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
};
uniform SpotLight spotLight;

uniform vec3 cameraPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec4 VertColor;

vec3 CalcDirLight(DirLight light, vec3 V, vec3 texel, vec3 N);
vec3 CalcPointLight(PointLight light, vec3 V, vec3 texel, vec3 N, vec3 P);
vec3 CalcSpotLight(SpotLight light, vec3 V, vec3 texel, vec3 N, vec3 P);

void main() {
    // vertex part
	vec3 worldPos = vec3(modelMatrix * vec4(aPos, 1.0));
    vec3 worldNormal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    // fragment part
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    vec3 V = normalize(worldPos - cameraPos);
    vec3 texel = vec3(texture(texture_diffuse0, aTexCoord));

    finalColor = texel;
    finalColor = clamp(finalColor, 0.0, 1.0);

    VertColor = vec4(finalColor, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 V, vec3 color, vec3 N) {
    vec3 L = -normalize(light.direction);
    float cosNL = max(dot(N, L), 0.0);
    vec3 R = 2 * dot(N, L) * N - L;
    float cosVRn = cosNL > 0.0 ? pow(max(dot(V, R), 0.0), material.shininess) : 0.0;

    vec3 ambient = light.ambient * material.ambient * color;
    vec3 diffuse = cosNL * light.diffuse * material.diffuse * color;
    vec3 specular = cosVRn * light.specular * material.specular * color;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 V, vec3 color, vec3 N, vec3 P) {
    vec3 L = normalize(light.position - P);
    float cosNL = max(dot(N, L), 0.0);
    vec3 R = 2 * dot(N, L) * N - L;
    float cosVRn = cosNL > 0.0 ? pow(max(dot(V, R), 0.0), material.shininess) : 0.0;

    float distance = length(light.position - P);
    float attenuation =
        1.0 / (light.k0 +
        light.k1 * distance +
        light.k2 * distance * distance);

    vec3 ambient = light.ambient * material.ambient * color;
    vec3 diffuse = cosNL * light.diffuse * material.diffuse * color;
    vec3 specular = cosVRn * light.specular * material.specular * color;
    return attenuation * (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 V, vec3 color, vec3 N, vec3 P) {
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

    vec3 ambient = light.ambient * material.ambient * color;
    vec3 diffuse = cosNL * light.diffuse * material.diffuse * color;
    vec3 specular = cosVRn * light.specular * material.specular * color;
    return attenuation * (ambient + diffuse + specular) * cosDLc;
}
