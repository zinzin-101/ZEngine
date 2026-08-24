#include "BasicEditableTerrain.h"
#include "../Camera.h"
#include "../Engine.h"
#include "../Ray.h"
#include "GLFW/glfw3.h"
#include <glad/glad.h>
#include <vector>

BasicEditableTerrain::BasicEditableTerrain(unsigned int width, unsigned int length, float spacing) :
    width(width), length(length), spacing(spacing),
    terrainStripCount(0), vertsPerTerrainStrip(0),
    ssbo(0), vao(0), vbo(0), ebo(0), vertsCount(0),
    shader(nullptr),
    horizontalScale(1.0f), verticalScale(1.0f), color(1.0f), radius(5.0f),
    isGrowing(true), growAmount(10.0f)
{}

BasicEditableTerrain::~BasicEditableTerrain() {
    glDeleteBuffers(1, &ssbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void BasicEditableTerrain::init() {
    computeShader.createShader("compute_shaders/terrain/basic_terrain_grow_shrink.comp");

    //..std::vector<float> verts; /// create a vector to store vertices for a flat x-z plane
    verts.clear();

    /// offsets so that the center is at origin
    float offsetX = static_cast<float>(width) / 2.0f;
    float offsetZ = static_cast<float>(length) / 2.0f;

    for (unsigned int x = 0; x < width; x++) {
        for (unsigned int z = 0; z < length; z++) {
            verts.emplace_back((static_cast<float>(x) - offsetX) * horizontalScale);
            verts.emplace_back(0.0f); // y is zero
            verts.emplace_back((static_cast<float>(z) - offsetZ) * horizontalScale);
        }
    }

    /// indices for rendering
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < width - 1; i++) {
        for (unsigned int j = 0; j < length; j++) {
            for (unsigned int k = 0; k < 2; k++) {
                indices.emplace_back(j + length * (i + k));
            }
        }
    }

    /// we're rendering in strips so we need to calculate the necessary information for rendering
    terrainStripCount = width - 1;
    vertsPerTerrainStrip = length * 2;

    /// storing vertices count to be used later in compute shader for height map edit
    vertsCount = static_cast<unsigned int>(verts.size() / 3);

    /// we need ssbo for calculation in compute shader, for this one it will be used to store the vertices data
    // create ssbo
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW); // initializing the ssbo with our vertices
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    /// opengl vao vbo and ebo binding for rendering
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
        glm::vec3 rayDir = Ray::getRayDirectionFromScreen(input->getMousePosition(), camera);

        ///..on mouse click
        /// 
        /// read data from ssbo
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, verts.size() * sizeof(float), verts.data()); // copying vertices data from ssbo to verts vector

        /// Defining a ray
        // camPos: position of the camera
        // rayDir: ray direction from the camera -> calculated from mouse position and projection matrix
        // 1000.0f: ray distance
        Ray ray = Ray(camPos, rayDir, 1000.0f);

        /// We will be iterating through all vertices to find the closest one to our ray
        int closestId = -1;
        float closestDist = FLT_MAX;

        glm::vec3 position = transform->getGlobalPosition();
        glm::vec3 rotation = transform->getGlobalEulerRotation();
        glm::vec3 scale = transform->getGlobalScale();
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));

        int n = static_cast<int>(verts.size()) / 3;
        for (int i = 0; i < n; i++) {
            glm::vec4 vert = model * glm::vec4(verts[3 * i], verts[3 * i + 1], verts[3 * i + 2], 1.0f); // don't forget to multiply by a model matrix
            float dist = ray.getDistanceFromPoint(vert); // calculating the closest distance from the ray to a vertex
            if (dist < closestDist && dist < radius) { // checking for bounds
                closestDist = dist;
                closestId = i;
            }
        }

        if (closestId == -1) return; // return if no valid vertex

        float dt = Engine::getInstance()->getDeltaTime();

        /// setting compute shader uniform variables
        computeShader.use();
        computeShader.setUInt("closestId", closestId); // closest vertex to ray
        computeShader.setFloat("radius", radius); // radius of effect
        computeShader.setUInt("vertsCount", vertsCount); // number of vertices
        computeShader.setBool("isGrow", isGrowing); // should increase or decrease height
        computeShader.setFloat("growAmount", growAmount * dt); // rate of increase/decrease, multiply by delta time

        /// dispatching the compute shader
        unsigned int workSize = 256; // how many thread per work group
        glDispatchCompute((vertsCount + workSize - 1) / workSize, 1, 1); // calculating the required number of workgroup
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waiting for all thread to finish modifying memory
    }
}

void BasicEditableTerrain::render() {
    if (shader == nullptr) return;

    /// binding our ssbo 
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo); // the ssbo contains our vertices data

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


    /// setting our MVP matrix
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    /// for visualization purpose
    shader->setVec3("color", color);
    shader->setFloat("maxHeight", static_cast<float>(width + length) * spacing * 0.0625); 

    /// rendering our height map
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