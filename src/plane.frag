#version 450 core

struct Light {
    vec3 position;
    vec3 color;
};

struct Material {
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

uniform Light light;
uniform Material material;
uniform vec3 view_pos;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

void main() {

    // ambient
    vec3 ambient = material.ambient * light.color;
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(light.position - FragPos).xyz;
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light.color * material.diffuse;
    // specular
    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 1.0);
    vec3 specular =  spec * light.color * material.specular;

    vec3 result = (ambient + diffuse + specular) * material.color;
    FragColor = vec4(result, 1.0);
}
