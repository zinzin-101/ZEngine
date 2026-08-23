#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout(std430, binding = 0) readonly buffer Vertices { float vertices[]; };

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

vec3 getVertexVec3(uint id) {
    uint base = id * 3;
    return vec3(vertices[base], vertices[base+1], vertices[base+2]);
}

void main() {
    vec4 pos = vec4(getVertexVec3(gl_VertexID), 1.0);
    FragPos = vec3(model * pos);
    //Normal = mat3(transpose(inverse(model))) * aNormal;
    Normal = vec3(0,1,0);
    gl_Position = projection * view * model * pos;
}