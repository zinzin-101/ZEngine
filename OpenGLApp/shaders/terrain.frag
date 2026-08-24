#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;
uniform float maxHeight;

void main() {
    float t = (FragPos.y - 0.0) / (maxHeight - 0.0);
    vec3 color = mix(vec3(0.0), color, clamp(t, 0.0, 1.0)); // for simple visualization purpose
    FragColor = vec4(color, 1.0);
}