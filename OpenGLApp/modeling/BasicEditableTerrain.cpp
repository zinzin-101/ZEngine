#include "BasicEditableTerrain.h"
#include <glad/glad.h>
#include <vector>

BasicEditableTerrain::BasicEditableTerrain(unsigned int width, unsigned int length, float spacing):
	width(width), length(length), spacing(spacing),
    terrainStripCount(0), vertsPerTerrainStrip(0),
	vao(0), vbo(0), ebo(0),
	horizontalScale(1.0f), verticalScale(1.0f)
{}

void BasicEditableTerrain::init() {
    std::vector<float> verts;
    float offsetX = static_cast<float>(width) / 2.0f;
    float offsetZ = static_cast<float>(length) / 2.0f;
    for (unsigned int x = 0; x < width; x++) {
        for (unsigned int z = 0; z < length; z++) {
            verts.emplace_back((static_cast<float>(x) - offsetX) * horizontalScale);
            verts.emplace_back(0.0f);
            verts.emplace_back((static_cast<float>(z) - offsetZ) * horizontalScale);
        }
    }

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < width - 1; i++) {
        for (unsigned int j = 0; j < length; j++) {
            for (unsigned int k = 0; k < 2; k++) {
                indices.emplace_back(j + length * (i + k));
            }
        }
    }

    terrainStripCount = width - 1;
    vertsPerTerrainStrip = length * 2;

    // bind VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // generate VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        verts.size() * sizeof(float),
        verts.data(),
        GL_STATIC_DRAW
    );

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // generate EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );
}
