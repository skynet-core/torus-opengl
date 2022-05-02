#version 440 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

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


in VS_OUT {
    vec4 fragmentPosition;
    vec4 normal;
    vec2 texCoord;
} gs_in[];

const float MAGNITUDE = 0.1;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = gl_in[index].gl_Position + gs_in[index].normal * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);// first vertex normal
    GenerateLine(1);// second vertex normal
    GenerateLine(2);// third vertex normal
}