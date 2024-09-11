#version 450 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct DirLight {
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};
uniform DirLight sun;
uniform DirLight moon;
uniform bool dayNightActive;

uniform vec3 tint;

uniform vec3 cameraPos;

uniform bool fogActive;
uniform vec3 fogColor;
uniform float fogIntensity;

in vec4 fragmentPosition;
in vec3 fragmentNormal;

out vec4 finalColor;

vec3 CalcDirLight(DirLight light, vec3 V, vec3 N);

void main()
{
    vec3 V = normalize(cameraPos - vec3(fragmentPosition));
    vec3 color = vec3(0, 0, 0);
    if (dayNightActive) {
        // sun
        color += CalcDirLight(sun, V, fragmentNormal);
    
        // moon
        color += CalcDirLight(moon, V, fragmentNormal);
    }
    finalColor = clamp(vec4(color, 1.0), 0, 1);
}

vec3 CalcDirLight(DirLight light, vec3 V, vec3 N) {
    vec3 L = -normalize(light.direction);
    float cosNL = max(dot(N, L), 0.0);
    vec3 R = 2 * dot(N, L) * N - L;
    float cosVRn = cosNL > 0.0 ? pow(max(dot(V, R), 0.0), material.shininess) : 0.0;

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = cosNL * light.diffuse * material.diffuse;
    vec3 specular = cosVRn * light.specular * material.specular;
    return light.color * (ambient + diffuse + specular) * tint;
}