#version 450 core

struct Light{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
    vec4 fragPosLightSpace;
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

#define MAX_SPOT_LIGHT 5
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
    mat4 lightSpaceMatrix;
};
uniform int spotLightCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHT];

struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
    mat4 lightSpaceMatrix;
};
uniform DirLight sun;

uniform vec3 cameraPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTriPos;
layout(location = 4) in vec3 aFaceNormal;

out Light PointLight1;
out Light PointLight2;
out Light SpotLight1;
out Light SpotLight2;
out Light Sun;
out vec2 TexCoords;
out vec3 FragPos;
out vec3 FragNormal;

Light CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 P);
Light CalcSpotLight(SpotLight light, vec3 V, vec3 N, vec3 P);
Light CalcDirLight(DirLight light, vec3 V, vec3 N);

void main() {
    // vertex part
    TexCoords = aTexCoord;
	vec3 worldPos = vec3(modelMatrix * vec4(aTriPos, 1.0));
    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    FragNormal = normalize(mat3(transpose(inverse(modelMatrix))) * aNormal);
    vec3 worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * aFaceNormal);
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    // fragment part
    vec3 V = normalize(cameraPos - worldPos);

    Light pl1 = CalcPointLight(pointLights[0], V, worldNormal, worldPos);
    pl1.position = pointLights[0].position;
    PointLight1 = pl1;

    Light pl2 = CalcPointLight(pointLights[1], V, worldNormal, worldPos);
    pl2.position = pointLights[1].position;
    PointLight2 = pl2;

    Light sl1 = CalcSpotLight(spotLights[0], V, worldNormal, worldPos);
    sl1.position = spotLights[0].position;
    sl1.fragPosLightSpace = spotLights[0].lightSpaceMatrix * vec4(FragPos, 1.0f);
    SpotLight1 = sl1;

    Light sl2 = CalcSpotLight(spotLights[1], V, worldNormal, worldPos);
    sl2.position = spotLights[1].position;
    sl2.fragPosLightSpace = spotLights[1].lightSpaceMatrix * vec4(FragPos, 1.0f);
    SpotLight2 = sl2;

    Light s = CalcDirLight(sun, V, worldNormal);
    s.position = -sun.direction;
    s.fragPosLightSpace = sun.lightSpaceMatrix * vec4(worldPos, 1.0f);
    Sun = s;
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
    res.ambient = vec4(light.ambient * material.ambient * attenuation, 1.0);
    res.diffuse = vec4(cosNL * light.diffuse * material.diffuse * attenuation, 1.0);
    res.specular = vec4(cosVRn * light.specular * material.specular * attenuation, 1.0);
    return res;
}

Light CalcSpotLight(SpotLight light, vec3 V, vec3 N, vec3 P) {
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

    Light res;
    res.ambient = vec4(light.ambient * material.ambient * attenuation * cosDLc, 1.0);
    res.diffuse = vec4(cosNL * light.diffuse * material.diffuse * attenuation * cosDLc, 1.0);
    res.specular = vec4(cosVRn * light.specular * material.specular * attenuation * cosDLc, 1.0);
    return res;
}

Light CalcDirLight(DirLight light, vec3 V, vec3 N) {
    vec3 L = -normalize(light.direction);
    float cosNL = max(dot(N, L), 0.0);
    vec3 R = 2 * dot(N, L) * N - L;
    float cosVRn = cosNL > 0.0 ? pow(max(dot(V, R), 0.0), material.shininess) : 0.0;

    Light res;
    res.ambient = vec4(light.ambient * material.ambient, 1.0);
    res.diffuse = vec4(cosNL * light.diffuse * material.diffuse, 1.0);
    res.specular = vec4(cosVRn * light.specular * material.specular, 1.0);
    return res;
}