#include "MeshPrimitive.h"
#include <glad/glad.h>

MeshPrimitive::MeshPrimitive():vao(0), vbo(0), ebo(0) {}

MeshPrimitive::~MeshPrimitive() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}