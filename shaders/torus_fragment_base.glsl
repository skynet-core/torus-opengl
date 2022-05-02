#version 440 core
out vec4 FragColor;

layout(std140) uniform Matrices {
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

layout(std140) uniform DirectionalLights {
    DirectionalLight lights[1];
} dirLights;

layout(std140) uniform PointLights {
    PointlLight lights[1];
} pointLights;

layout(std140) uniform SpotLights {
    SpotLight lights[1];
} spotLights;

in VS_OUT {
    vec4 fragmentPosition;
    vec4 normal;
    vec2 texCoord;
} fs_in;

vec4 CalcDirLight(DirectionalLight light, vec4 normal, vec4 viewDir, vec4 ambient, vec4 diffuse, vec4 specular) {
    vec4 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec4 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec4 ambC = light.ambient * ambient;
    vec4 diffC = light.diffuse * diff * diffuse;
    vec4 specC = light.specular * spec * specular;
    return (ambC + diffC + specC);
}

void main() {
    vec4 result = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    vec4 viewPos = vec4(0.0f, 0.0f, 10.0f, 1.0f);
    vec4 viewDir = normalize(viewPos - fs_in.fragmentPosition);
    vec4 ambient = material.ambient;
    vec4 diffuse = material.diffuse;
    vec4 specular = material.specular;
    for(int i = 0; i < 1; i++) {
        result += CalcDirLight(dirLights.lights[i], fs_in.normal, viewDir, ambient, diffuse, specular);
    }
    FragColor = result;
    // FragColor = ambient + diffuse + specular;
}
