#version 440 core
out vec4 FragColor;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
    mat4 model;
    mat4 normalMatrix;
} matricies;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    sampler2D diffuseTex;
    sampler2D specularTex;
    float shininess;
    float metalness;
    bool textured;
};

struct DirectionalLight {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct PointlLight {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant;
    float linear;
    float quadratic;
// padding 4
};

struct SpotLight {
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
// padding 12
};

uniform Material material;

layout (std140) uniform DirectionalLights {
    DirectionalLight lights[1];
} dirLights;

layout (std140) uniform PointLights {
    PointlLight lights[1];
} pointLights;

layout (std140) uniform SpotLights {
    SpotLight lights[1];
} spotLights;

void main() {
    FragColor = material.ambient;
}
