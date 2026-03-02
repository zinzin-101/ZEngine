#include "TetrahedronSoftBodyMesh.h"
#include "Engine.h"
#include <glad/glad.h>

TetrahedronSoftBodyMesh::TetrahedronSoftBodyMesh() : tempVerticesData{ 0.0f } {
	float vertices[] = {
        // Position            // Normal
        -1.0f, -1.0f,  1.0f,   -0.577f, -0.577f,  0.577f,  // v0
         1.0f, -1.0f, -1.0f,    0.577f, -0.577f, -0.577f,  // v1
        -1.0f,  1.0f, -1.0f,   -0.577f,  0.577f, -0.577f,  // v2
         1.0f,  1.0f,  1.0f,    0.577f,  0.577f,  0.577f   // v3
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2
    };

    unsigned int edgeIdx[] = {
        0, 1,
        0, 2,
        0, 3,
        1, 2,
        1, 3,
        2, 3
    };

    unsigned int volumeIdx[] = {
        1, 3, 2,
        0, 2, 3,
        0, 3, 1,
        0, 1, 2
    };

    int n = 4 * 6;
    for (int i = 0; i < n; i++) {
        tetrahedronVertices[i] = vertices[i];
    }
    n = 4 * 3;
    for (int i = 0; i < n; i++) {
        tetrahedronIndices[i] = indices[i];
    }
    n = 12;
    for (int i = 0; i < n; i++) {
        tetrahedronEdgeId[i] = edgeIdx[i];
    }

    for (int i = 0; i < 4; i++) {
        volumeIdxOrder[i][0] = volumeIdx[3 * i + 0];
        volumeIdxOrder[i][1] = volumeIdx[3 * i + 1];
        volumeIdxOrder[i][2] = volumeIdx[3 * i + 2];
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); // use dynamic draw due to frequent updating

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0
    );
    glEnableVertexAttribArray(0);

    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // generate EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    color = glm::vec3(1.0f);
}

float TetrahedronSoftBodyMesh::getTetrahedronVolume() const {
    int idx0 = 0;
    int idx1 = 1;
    int idx2 = 2;
    int idx3 = 3;

    glm::vec3 p0 = getPosFromIdx(idx0);
    glm::vec3 p1 = getPosFromIdx(idx1);
    glm::vec3 p2 = getPosFromIdx(idx2);
    glm::vec3 p3 = getPosFromIdx(idx3);

    glm::vec3 edge1 = p1 - p0;
    glm::vec3 edge2 = p2 - p0;
    glm::vec3 edge3 = p3 - p0;

    float volume = glm::dot(glm::cross(edge1, edge2), edge3) / 6.0f;
    return volume;
}

glm::vec3 TetrahedronSoftBodyMesh::getPosFromIdx(int idx) const {
    //int base = idx * 6; // 6 floats per vertex
    //return glm::vec3(
    //    tetrahedronVertices[base],
    //    tetrahedronVertices[base + 1],
    //    tetrahedronVertices[base + 2]
    //);
    return particlePositions[idx];
}

void TetrahedronSoftBodyMesh::computeNormal() {
    for (glm::vec3& normal : tetrahedronNormals) {
        normal = glm::vec3(0.0f);
    }

    for (int i = 0; i < 4; i++) {
        unsigned int idx0 = tetrahedronIndices[3 * i + 0];
        unsigned int idx1 = tetrahedronIndices[3 * i + 1];
        unsigned int idx2 = tetrahedronIndices[3 * i + 2];

        glm::vec3 p0 = particlePositions[idx0];
        glm::vec3 p1 = particlePositions[idx1];
        glm::vec3 p2 = particlePositions[idx2];

        glm::vec3 normal = computeFaceNormal(p0, p1, p2);
        tetrahedronNormals[idx0] += normal;
        tetrahedronNormals[idx1] += normal;
        tetrahedronNormals[idx2] += normal;
    }

    for (glm::vec3& normal : tetrahedronNormals) {
        normal = glm::normalize(normal);
    }
}

void TetrahedronSoftBodyMesh::preSolve(float dt, glm::vec3 gravity) {
    for (int i = 0; i < numberOfParticles; i++) {
        if (invMasses[i] == 0.0f) continue;
        particleVelocities[i] += gravity * dt;
        previousParticlePositions[i] = particlePositions[i];
        particlePositions[i] += particleVelocities[i] * dt;
        if (particlePositions[i].y < groundHeight) {
            particlePositions[i] = previousParticlePositions[i];
            particlePositions[i].y = groundHeight;
        }
    }
}

void TetrahedronSoftBodyMesh::solve(float dt) {
    solveEdges(edgeCompliance, dt);
    solveVolumes(volumeCompliance, dt);
}

void TetrahedronSoftBodyMesh::postSolve(float dt) {
    for (int i = 0; i < numberOfParticles; i++) {
        if (invMasses[i] == 0.0f) continue;
        particleVelocities[i] = (particlePositions[i] - previousParticlePositions[i]) / dt;
    }
    updateMesh();
}

void TetrahedronSoftBodyMesh::solveEdges(float compliance, float dt) {
    float alpha = compliance / (dt * dt);
    for (int i = 0; i < edgeLengths.size(); i++) {
        int idx0 = tetrahedronEdgeId[2 * i + 0];
        int idx1 = tetrahedronEdgeId[2 * i + 1];
        float w0 = invMasses[idx0];
        float w1 = invMasses[idx1];
        float w = w0 + w1;
        if (w == 0.0f) continue;

        glm::vec3& p0 = particlePositions[idx0];
        glm::vec3& p1 = particlePositions[idx1];
        glm::vec3 gradient = p0 - p1;
        float gradientLength = glm::length(gradient);
        if (gradientLength == 0.0f) continue;
        gradient = glm::normalize(gradient);
        float restLength = edgeLengths[i];
        float c = gradientLength - restLength;
        float s = -c / (w + alpha);
        p0 += gradient * s * w0;
        p1 += gradient * s * w1;
    }
}

void TetrahedronSoftBodyMesh::solveVolumes(float compliance, float dt) {
    float alpha = compliance / (dt * dt);
    for (int i = 0; i < numberOfTetrahedrons; i++) {
        float w = 0.0f;
        //int indices[4] = { 0, 1, 2, 3, };
        glm::vec3 p0 = particlePositions[0];
        glm::vec3 p1 = particlePositions[1];
        glm::vec3 p2 = particlePositions[2];
        glm::vec3 p3 = particlePositions[3];
        glm::vec3 grads[4];
        grads[0] = -glm::cross(p1 - p2, p3 - p2) / 6.0f;
        grads[1] = glm::cross(p2 - p0, p3 - p0) / 6.0f;
        grads[2] = glm::cross(p3 - p0, p1 - p0) / 6.0f;
        grads[3] = glm::cross(p1 - p0, p2 - p0) / 6.0f;
        for (int j = 0; j < 4; j++) {
            //int idx0 = indices[volumeIdxOrder[j][0]];
            //int idx1 = indices[volumeIdxOrder[j][1]];
            //int idx2 = indices[volumeIdxOrder[j][2]];

            //glm::vec3 p0 = particlePositions[idx0];
            //glm::vec3 p1 = particlePositions[idx1];
            //glm::vec3 p2 = particlePositions[idx2];

            //glm::vec3 edge1 = p1 - p0;
            //glm::vec3 edge2 = p2 - p0;

            //grads[j] = glm::cross(edge1, edge2) / 6.0f;
            //w += invMasses[indices[j]] * glm::dot(grads[j], grads[j]);
            w += invMasses[j] * glm::dot(grads[j], grads[j]);
        }

        if (w == 0.0f) continue;

        float volume = getTetrahedronVolume();
        float restVolume = restVolumes[i];
        float c = volume - restVolume;
        float s = -c / (w + alpha);
        for (int j = 0; j < 4; j++) {
            //int idx = indices[j];
            //particlePositions[idx] += grads[j] * s * invMasses[idx];
            particlePositions[j] += grads[j] * s * invMasses[j];
        }

        glm::vec3 sum = grads[0] + grads[1] + grads[2] + grads[3];
        std::cout << "Grad sum: "
            << sum.x << " "
            << sum.y << " "
            << sum.z << std::endl;
    }
}

void TetrahedronSoftBodyMesh::updateMesh() {
    for (int i = 0; i < numberOfParticles; i++) {
        glm::vec3 position = transform->getGlobalPosition();
        tempVerticesData[6 * i + 0] = particlePositions[i].x + position.x;
        tempVerticesData[6 * i + 1] = particlePositions[i].y + position.y;
        tempVerticesData[6 * i + 2] = particlePositions[i].z + position.z;
    }
    computeNormal();
    for (int i = 0; i < numberOfParticles; i++) {
        glm::vec3 position = transform->getGlobalPosition();
        tempVerticesData[6 * i + 3] = tetrahedronNormals[i].x;
        tempVerticesData[6 * i + 4] = tetrahedronNormals[i].y;
        tempVerticesData[6 * i + 5] = tetrahedronNormals[i].z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numberOfParticles * 6 * sizeof(float), tempVerticesData);
}

void TetrahedronSoftBodyMesh::init() {
    numberOfParticles = 4;
    numberOfTetrahedrons = 1;
    numberOfEdges = 6;
    for (int i = 0; i < numberOfParticles; i++) {
        glm::vec3 pos = glm::vec3(
            tetrahedronVertices[i * 6 + 0],
            tetrahedronVertices[i * 6 + 1],
            tetrahedronVertices[i * 6 + 2]
        );
        glm::vec3 normal = glm::vec3(
            tetrahedronVertices[i * 6 + 3],
            tetrahedronVertices[i * 6 + 4],
            tetrahedronVertices[i * 6 + 5]
        );
        particlePositions.emplace_back(pos);
        tetrahedronNormals.emplace_back(normal);
        previousParticlePositions.emplace_back(pos);
        particleVelocities.emplace_back(glm::vec3(0.0f));
        invMasses.emplace_back(0.0f);
    }
    for (int i = 0; i < numberOfTetrahedrons; i++) {
        restVolumes.emplace_back(0.0f);
    }

    for (int i = 0; i < numberOfEdges; i++) {
        edgeLengths.emplace_back(0.0f);
    }


    float volume = getTetrahedronVolume();
    float pInvMass = volume > 0.0f ? (1.0f / (volume / 4.0f)) : 0.0f;
    invMasses[0] += pInvMass;
    invMasses[1] += pInvMass;
    invMasses[2] += pInvMass;
    invMasses[3] += pInvMass;

    for (int i = 0; i < numberOfEdges; i++) {
        int idx0 = tetrahedronEdgeId[2 * i + 0];
        int idx1 = tetrahedronEdgeId[2 * i + 1];
        edgeLengths[i] = glm::length(particlePositions[idx0] - particlePositions[idx1]);
    }

    restVolumes[0] = volume;
}

void TetrahedronSoftBodyMesh::update() {
    float dt = Engine::getInstance()->getTime()->getDeltaTime();
    if (dt <= 0.001f) return;
    //glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
    glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
    float subDt = dt / substeps;
    for (unsigned int i = 0; i < substeps; i++) {
        preSolve(subDt, gravity);
        solve(subDt);
        postSolve(subDt);
        for (int i = 0; i < numberOfParticles; i++) {
            if (glm::any(glm::isnan(particlePositions[i]))) {
                //std::cout << "NaN position!\n";
            }
            else {
                //std::cout << i << ": " << particlePositions[i].x << " " << particlePositions[i].y << " " << particlePositions[i].z << std::endl;
            }
        }
    }
}

void TetrahedronSoftBodyMesh::render() {
    shader->use();
    Camera* camera = Engine::getInstance()->getCurrentScene()->getCurrentCamera();
    glm::vec3 position = transform->getGlobalPosition();
    glm::vec3 rotation = transform->getGlobalEulerRotation();
    glm::vec3 scale = transform->getGlobalScale();

    glm::mat4 model(1.0f);
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
    shader->setVec3("camPos", camera->getTransform()->getGlobalPosition());

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}