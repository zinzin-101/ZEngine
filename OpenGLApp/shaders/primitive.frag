#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;

vec3 lightDir = normalize(vec3(1.0, 1.0, 0.5));
vec3 lightColor = vec3(1.0);

void main() {
    // Diffuse lighting
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Small ambient term
    vec3 ambient = 0.1 * lightColor;

    vec3 result = (ambient + diffuse) * color;

    FragColor = vec4(result, 1.0);
    //FragColor = vec4(color, 1.0);
}