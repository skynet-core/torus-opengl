#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

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


out VS_OUT {
    vec4 fragmentPosition;
    vec4 normal;
    vec2 texCoord;
} vs_out;

void main() {
    gl_Position = matricies.projection * matricies.view * matricies.model * vec4(aPos, 1.0f);
    vs_out.normal = normalize(matricies.normalMatrix * vec4(aNorm, 1.0f));
    vs_out.texCoord = aTexCoord;
    vs_out.fragmentPosition = matricies.model * vec4(aPos, 1.0f);
}
