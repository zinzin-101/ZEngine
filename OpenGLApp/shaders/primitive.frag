#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;
uniform vec3 camPos;

vec3 lightDir = normalize(vec3(-0.0, -1.0, -0.0));

float shininess = 48;
vec3 ambient = vec3(0.1);
vec3 diffuse = vec3(0.5);
vec3 specular = vec3(0.5);

vec3 CalcDirLight(vec3 lightDir, vec3 normal, vec3 viewDir);

void main() {
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 result = CalcDirLight(lightDir, Normal, viewDir);

    FragColor = vec4(result, 1.0);
    FragColor = vec4(color, 1.0);
}

vec3 CalcDirLight(vec3 lightDirection, vec3 normal, vec3 viewDirection) {
    vec3 lightDir = normalize(-lightDirection);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), shininess);
    // combine results
    vec3 _ambient = ambient * color;
    vec3 _diffuse = diffuse * diff * color;
    vec3 _specular = specular * spec * color;
    return (_ambient + _diffuse + _specular);
}