#include "BasicEditableTerrain.h"
#include "../Camera.h"
#include "../Engine.h"
#include "GLFW/glfw3.h"
#include <glad/glad.h>
#include <vector>

BasicEditableTerrain::BasicEditableTerrain(unsigned int width, unsigned int length, float spacing) :
    width(width), length(length), spacing(spacing),
    terrainStripCount(0), vertsPerTerrainStrip(0),
    ssbo(0), vao(0), vbo(0), ebo(0), vertsCount(0),
    shader(nullptr),
    horizontalScale(1.0f), verticalScale(1.0f), color(1.0f), radius(5.0f),
    isGrowing(true), growAmount(2.5f)
{}

BasicEditableTerrain::~BasicEditableTerrain() {
    glDeleteBuffers(1, &ssbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void BasicEditableTerrain::init() {
    computeShader.createShader("../compute_shaders/terrain/basic_terrain_grow_shrink.comp");

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

    vertsCount = static_cast<unsigned int>(verts.size() / 3);

    // create ssbo
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

void BasicEditableTerrain::update() {
    if (computeShader.ID == 0) return;

    InputManager* input = Engine::getInstance()->getInputManager();
    if (input->getMouse(GLFW_MOUSE_BUTTON_LEFT)) {
        Camera* camera = Engine::getInstance()->getCurrentScene()->getCurrentCamera();
        glm::vec3 camPos = camera->getTransform()->getGlobalPosition();
        glm::vec3 camDir = camera->getFoward();

        float dt = Engine::getInstance()->getDeltaTime();

        computeShader.use();
        computeShader.setVec3("camPos", camPos);
        computeShader.setVec3("camDir", camDir);
        computeShader.setFloat("rayDist", 1000.0f);
        computeShader.setFloat("radius", radius);
        computeShader.setUInt("vertsCount", vertsCount);
        computeShader.setBool("isGrow", isGrowing);
        computeShader.setFloat("growAmount", growAmount * dt);

        unsigned int workSize = 256;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
        glDispatchCompute((vertsCount + workSize - 1) / workSize, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void BasicEditableTerrain::render() {
    if (shader == nullptr) return;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    shader->use();
    Camera* camera = Engine::getInstance()->getCurrentScene()->getCurrentCamera();
    glm::vec3 position = transform->getGlobalPosition();
    glm::vec3 rotation = transform->getGlobalEulerRotation();
    glm::vec3 scale = transform->getGlobalScale();

    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));

    glm::mat4 view = camera->getViewMatrix();

    glm::vec2 screenDimension = Engine::getInstance()->getScreenDimension();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera->fov),
            (float)screenDimension.x / (float)screenDimension.y,
            camera->nearPlane, camera->farPlane);

    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("color", color);
    shader->setFloat("maxHeight", 5.0f); // for visualization purpose

    glBindVertexArray(vao);
    for (unsigned int i = 0; i < terrainStripCount; i++) {
        glDrawElements(
            GL_TRIANGLE_STRIP,
            vertsPerTerrainStrip,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * vertsPerTerrainStrip * i)
        );
    }
}