#include "Mesh.h"
#include "Transform.h"

using namespace MeshConfig;

unsigned int Mesh::defaultAlbedo = 0;
unsigned int Mesh::defaultNormal = 0;
unsigned int Mesh::defaultMetallic = 0;
unsigned int Mesh::defaultRoughness = 0;
unsigned int Mesh::defaultAO = 0;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    if (defaultAlbedo == 0) defaultAlbedo = createDefaultTexture(255, 255, 255);
    if (defaultNormal == 0) defaultNormal = createDefaultTexture(128, 128, 255);
    if (defaultMetallic == 0) defaultMetallic = createDefaultTexture(255, 255, 0);
    if (defaultRoughness == 0) defaultRoughness = createDefaultTexture(255, 255, 255);
    if (defaultAO == 0) defaultAO = createDefaultTexture(255, 255, 255);

    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::draw(Shader& shader, glm::vec2 tiling) {
    // bind textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    // PBR
    unsigned int diffuseNrPBR = 1;
    unsigned int normalNrPBR = 1;
    unsigned int metallicNrPBR = 1;
    unsigned int roughnessNrPBR = 1;
    unsigned int aoNrPBR = 1;

    // reset all textures
    for (unsigned int i = 0; i < 5; i++) {
        unsigned int defaultTex[] = {
            defaultAlbedo,
            defaultNormal,
            defaultMetallic,
            defaultRoughness,
            defaultAO
        };

        glActiveTexture(STARTING_TEXTURE_GL_INDEX + i);
        glBindTexture(GL_TEXTURE_2D, defaultTex[i]);
    }

    unsigned int slot = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse") {
            //number = std::to_string(diffuseNr++);
            name = "albedoMap";
            slot = ALBEDO_SLOT;
            number = std::to_string(diffuseNrPBR++);
        }
        else if (name == "texture_specular") {
            name = "metallicMap";
            slot = METALLIC_SLOT;
            number = std::to_string(metallicNrPBR++);
        }
        else if (name == "texture_normal") {
            name = "normalMap";
            slot = NORMAL_SLOT;
            number = std::to_string(normalNrPBR++);
        }
        else if (name == "texture_height") {
            name = "roughnessMap";
            slot = ROUGHNESS_SLOT;
            number = std::to_string(roughnessNrPBR++);
        }
        // PBR
        else if (name == "texture_PBR_diffuse") {
            name = "albedoMap";
            slot = ALBEDO_SLOT;
            number = std::to_string(diffuseNrPBR++);
        }
        else if (name == "texture_PBR_normal") {
            name = "normalMap";
            slot = NORMAL_SLOT;
            number = std::to_string(normalNrPBR++);
        }
        else if (name == "texture_PBR_metallic") {
            name = "metallicMap";
            slot = METALLIC_SLOT;
            number = std::to_string(metallicNrPBR++);
        }
        else if (name == "texture_PBR_roughness") {
            name = "roughnessMap";
            slot = ROUGHNESS_SLOT;
            number = std::to_string(roughnessNrPBR++);
        }
        else if (name == "texture_PBR_ambient_occlusion") {
            name = "aoMap";
            slot = AO_SLOT;
            number = std::to_string(aoNrPBR++);
        }

        if (slot < STARTING_TEXTURE_INDEX) {
            std::cout << "Texture type is incompatible with Mesh::Draw()" << std::endl;
            continue;
        }

        glActiveTexture(GL_TEXTURE0 + slot);
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), slot);
        glUniform1i(glGetUniformLocation(shader.ID, "useMR"), (metallicNrPBR != 1 && roughnessNrPBR == 1) ? 1 : 0);
        glUniform2fv(glGetUniformLocation(shader.ID, "tiling"), 1, &tiling[0]);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::drawGeometry() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    // bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIds));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    glBindVertexArray(0);
}

unsigned int Mesh::createDefaultTexture(unsigned char r, unsigned char g, unsigned char b) {
    unsigned int tex;
    unsigned char data[3] = { r, g, b };

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        1, 1,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}