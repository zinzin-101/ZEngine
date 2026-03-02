#include "TetrahedronSoftBodyMesh.h"
#include "Engine.h"
#include <glad/glad.h>

using namespace SoftBodyVectorOperation;

TetrahedronSoftBodyMesh::TetrahedronSoftBodyMesh() : tempVerticesData{ 0.0f } {
    numberOfParticles = 4;
    numberOfTetrahedrons = 1;

    float vertices[] = {
        -1.0f, 0.0f,  1.0f,   0.0f, -1.0f, 0.0f,
         1.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,

        -1.0f, 0.0f,  1.0f,  -0.577f, 0.577f, 0.577f,
         0.0f, 2.0f,  0.0f,  -0.577f, 0.577f, 0.577f,
         1.0f, 0.0f, -1.0f,  -0.577f, 0.577f, 0.577f,

          1.0f, 0.0f, -1.0f,   0.577f, 0.577f, -0.577f,
          0.0f, 2.0f,  0.0f,   0.577f, 0.577f, -0.577f,
         -1.0f, 0.0f, -1.0f,   0.577f, 0.577f, -0.577f,

         -1.0f, 0.0f, -1.0f,   -0.577f, 0.577f, -0.577f,
          0.0f, 2.0f,  0.0f,   -0.577f, 0.577f, -0.577f,
         -1.0f, 0.0f,  1.0f,   -0.577f, 0.577f, -0.577f
    };

    for (int i = 0; i < numberOfParticles * 3; i++) {
        renderVertices.emplace_back(vertices[6 * i + 0]);
        renderVertices.emplace_back(vertices[6 * i + 1]);
        renderVertices.emplace_back(vertices[6 * i + 2]);
        renderVertices.emplace_back(vertices[6 * i + 3]);
        renderVertices.emplace_back(vertices[6 * i + 4]);
        renderVertices.emplace_back(vertices[6 * i + 5]);
    }

    float uniqueVerts[] = {
        -1.0f,  0.0f,  1.0f,
         1.0f,  0.0f, -1.0f,
        -1.0f,  0.0f, -1.0f,
         0.0f,  2.0f,  0.0f 
    };

    for (int i = 0; i < numberOfParticles; i++) {
        particlePositions.emplace_back(uniqueVerts[3 * i + 0]);
        particlePositions.emplace_back(uniqueVerts[3 * i + 1]);
        particlePositions.emplace_back(uniqueVerts[3 * i + 2]);

        previousParticlePositions.emplace_back(uniqueVerts[3 * i + 0]);
        previousParticlePositions.emplace_back(uniqueVerts[3 * i + 1]);
        previousParticlePositions.emplace_back(uniqueVerts[3 * i + 2]);
    }

    unsigned int indices[] = { 
        0, 1, 2, 3
    };

    for (int i = 0; i < numberOfTetrahedrons; i++) {
        tetrahedronIndices.emplace_back(indices[4 * i + 0]);
        tetrahedronIndices.emplace_back(indices[4 * i + 1]);
        tetrahedronIndices.emplace_back(indices[4 * i + 2]);
        tetrahedronIndices.emplace_back(indices[4 * i + 3]);
    }

    unsigned int edgeIdx[] = {
        0, 1,
        0, 2,
        0, 3,
        1, 2,
        1, 3,
        2, 3
    };

    for (int i = 0; i < 6; i++) {
        edgeIndices.emplace_back(edgeIdx[2 * i + 0]);
        edgeIndices.emplace_back(edgeIdx[2 * i + 1]);
    }

    unsigned int volumeIdxOrder[] = {
        1, 3, 2,
        0, 2, 3,
        0, 3, 1,
        0, 1, 2
    };

    for (int i = 0; i < numberOfParticles; i++) {
        volumeIndicesOrder.emplace_back(volumeIdxOrder[3 * i + 0]);
        volumeIndicesOrder.emplace_back(volumeIdxOrder[3 * i + 1]);
        volumeIndicesOrder.emplace_back(volumeIdxOrder[3 * i + 2]);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, renderVertices.size() * sizeof(float), renderVertices.data(), GL_DYNAMIC_DRAW); // use dynamic draw due to frequent updating

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0
    );
    glEnableVertexAttribArray(0);

    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // generate EBO
    //glGenBuffers(1, &ebo);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    color = glm::vec3(1.0f);
}

void TetrahedronSoftBodyMesh::reset() {
    float uniqueVerts[] = {
    -1.0f,  0.0f,  1.0f,
     1.0f,  0.0f, -1.0f,
    -1.0f,  0.0f, -1.0f,
     0.0f,  2.0f,  0.0f
    };

    int n = (int)particlePositions.size() / 3;
    for (int i = 0; i < n; i++) {
        particlePositions[3 * i + 0] = uniqueVerts[3 * i + 0];
        particlePositions[3 * i + 1] = uniqueVerts[3 * i + 1];
        particlePositions[3 * i + 2] = uniqueVerts[3 * i + 2];

        previousParticlePositions[3 * i + 0] = uniqueVerts[3 * i + 0];
        previousParticlePositions[3 * i + 1] = uniqueVerts[3 * i + 1];
        previousParticlePositions[3 * i + 2] = uniqueVerts[3 * i + 2];

        particleVelocities[3 * i + 0] = 0.0f;
        particleVelocities[3 * i + 1] = 0.0f;
        particleVelocities[3 * i + 2] = 0.0f;
    }
}

float TetrahedronSoftBodyMesh::getTetrahedronVolume(int nr) {
    int idx0 = tetrahedronIndices[4 * nr + 0];
    int idx1 = tetrahedronIndices[4 * nr + 1];
    int idx2 = tetrahedronIndices[4 * nr + 2];
    int idx3 = tetrahedronIndices[4 * nr + 3];
    vecSetDiff(temp, 0, particlePositions.data(), idx1, particlePositions.data(), idx0);
    vecSetDiff(temp, 1, particlePositions.data(), idx2, particlePositions.data(), idx0);
    vecSetDiff(temp, 2, particlePositions.data(), idx3, particlePositions.data(), idx0);
    vecSetCross(temp, 3, temp, 0, temp, 1);
    return vecDot(temp, 3, temp, 2) / 6.0f;
}

void TetrahedronSoftBodyMesh::preSolve(float dt, glm::vec3 gravity) {
    glm::vec3 position = transform->getGlobalPosition();

    float tempGravity[] = { gravity.x, gravity.y, gravity.z };
    for (int i = 0; i < numberOfParticles; i++) {
        if (isNear(invMasses[i], 0.0f)) continue;

        vecAdd(particleVelocities.data(), i, tempGravity, 0, dt);
        vecCopy(previousParticlePositions.data(), i, particlePositions.data(), i);
        vecAdd(particlePositions.data(), i, particleVelocities.data(), i, dt);
        float y = particlePositions[3 * i + 1];
        if (y < groundHeight - position.y) {
            vecCopy(particlePositions.data(), i, previousParticlePositions.data(), i);
            particlePositions[3 * i + 1] = groundHeight - position.y;
        }
    }
}

void TetrahedronSoftBodyMesh::solve(float dt) {
    solveEdges(edgeCompliance, dt);
    solveVolumes(volumeCompliance, dt);
}

void TetrahedronSoftBodyMesh::postSolve(float dt) {
    for (int i = 0; i < numberOfParticles; i++) {
        if (isNear(invMasses[i], 0.0f)) continue;
        vecSetDiff(particleVelocities.data(), i, particlePositions.data(), i, previousParticlePositions.data(), i, 1.0f / dt);
    }
    updateMesh();
}

void TetrahedronSoftBodyMesh::solveEdges(float compliance, float dt) {
    float alpha = compliance / (dt * dt);
    int n = (int)edgeLengths.size();
    for (int i = 0; i < n; i++) {
        int idx0 = edgeIndices[2 * i + 0];
        int idx1 = edgeIndices[2 * i + 1];
        float w0 = invMasses[idx0];
        float w1 = invMasses[idx1];
        float w = w0 + w1;
        if (isNear(w, 0.0f)) continue;

        vecSetDiff(grads, 0, particlePositions.data(), idx0, particlePositions.data(), idx1);
        float length = glm::sqrt(vecLengthSquared(grads, 0));
        if (isNear(length, 0.0f)) continue;

        vecScale(grads, 0, 1.0f / length);
        float restLength = edgeLengths[i];
        float C = length - restLength;
        float s = -C / (w + alpha);
        vecAdd(particlePositions.data(), idx0, grads, 0, s * w0);
        vecAdd(particlePositions.data(), idx1, grads, 0, -s * w1);
    }
}

void TetrahedronSoftBodyMesh::solveVolumes(float compliance, float dt) {
    float alpha = compliance / (dt * dt);
    for (int i = 0; i < numberOfTetrahedrons; i++) {
        float w = 0.0f;
        for (int j = 0; j < 4; j++) {
            int idx0 = tetrahedronIndices[4 * i + volumeIndicesOrder[3 * j + 0]];
            int idx1 = tetrahedronIndices[4 * i + volumeIndicesOrder[3 * j + 1]];
            int idx2 = tetrahedronIndices[4 * i + volumeIndicesOrder[3 * j + 2]];

            vecSetDiff(temp, 0, particlePositions.data(), idx1, particlePositions.data(), idx0);
            vecSetDiff(temp, 1, particlePositions.data(), idx2, particlePositions.data(), idx0);
            vecSetCross(grads, j, temp, 0, temp, 1);
            vecScale(grads, j, 1.0f / 6.0f);

            w += invMasses[tetrahedronIndices[4 * i + j]] * vecLengthSquared(grads, j);
        }

        if (isNear(w, 0.0f)) continue;

        float volume = getTetrahedronVolume(i);
        float restVolume = restVolumes[i];
        float C = volume - restVolume;
        float s = -C / (w + alpha);
        for (int j = 0; j < 4; j++) {
            int idx = tetrahedronIndices[4 * i + j];
            vecAdd(particlePositions.data(), idx, grads, j, s * invMasses[idx]);
        }
    }
}

void TetrahedronSoftBodyMesh::computeRenderingNormal() {
    for (int face = 0; face < 4; face++) {
        int base = face * 3;

        glm::vec3 v0(
            renderVertices[(base + 0) * 6 + 0],
            renderVertices[(base + 0) * 6 + 1],
            renderVertices[(base + 0) * 6 + 2]
        );

        glm::vec3 v1(
            renderVertices[(base + 1) * 6 + 0],
            renderVertices[(base + 1) * 6 + 1],
            renderVertices[(base + 1) * 6 + 2]
        );

        glm::vec3 v2(
            renderVertices[(base + 2) * 6 + 0],
            renderVertices[(base + 2) * 6 + 1],
            renderVertices[(base + 2) * 6 + 2]
        );

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        for (int i = 0; i < 3; i++)
        {
            renderVertices[(base + i) * 6 + 3] = normal.x;
            renderVertices[(base + i) * 6 + 4] = normal.y;
            renderVertices[(base + i) * 6 + 5] = normal.z;
        }
    }
}

void TetrahedronSoftBodyMesh::updateMesh() {
    //glm::vec3 position = transform->getGlobalPosition();
    for (int i = 0; i < 12; i++) {
        int index = volumeIndicesOrder[i];
        renderVertices[i * 6 + 0] = particlePositions[index * 3 + 0];// + position.x;
        renderVertices[i * 6 + 1] = particlePositions[index * 3 + 1];// + position.y;
        renderVertices[i * 6 + 2] = particlePositions[index * 3 + 2];// + position.z;
    }
    computeRenderingNormal();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderVertices.size() * sizeof(float), renderVertices.data());
}

void TetrahedronSoftBodyMesh::init() {
    for (int i = 0; i < numberOfTetrahedrons; i++) {
        restVolumes.emplace_back(0.0f);
    }

    int n = (int)edgeIndices.size() / 2;
    for (int i = 0; i < n; i++) {
        edgeLengths.emplace_back(0.0f);
    }

    for (int i = 0; i < numberOfParticles; i++) {
        invMasses.emplace_back(0.0f);

        particleVelocities.emplace_back(0.0f);
        particleVelocities.emplace_back(0.0f);
        particleVelocities.emplace_back(0.0f);
    }

    n = 4 * 3;
    for (int i = 0; i < n; i++) {
        temp[i] = 0.0f;
        grads[i] = 0.0f;
    }

    for (int i = 0; i < numberOfTetrahedrons; i++) {
        float volume = getTetrahedronVolume(i);
        restVolumes[i] = volume;
        float particleInvMass = volume > 0.0f ? (1.0f / (volume / 4.0f)) : 0.0f;
        invMasses[tetrahedronIndices[4 * i + 0]] += particleInvMass;
        invMasses[tetrahedronIndices[4 * i + 1]] += particleInvMass;
        invMasses[tetrahedronIndices[4 * i + 2]] += particleInvMass;
        invMasses[tetrahedronIndices[4 * i + 3]] += particleInvMass;
    }

    n = (int)edgeLengths.size();
    for (int i = 0; i < n; i++) {
        int idx0 = edgeIndices[2 * i + 0];
        int idx1 = edgeIndices[2 * i + 1];
        edgeLengths[i] = glm::sqrt(vecDistSquared(particlePositions.data(), idx0, particlePositions.data(), idx1));
    }
}

void TetrahedronSoftBodyMesh::update() {
    float dt = Engine::getInstance()->getTime()->getDeltaTime();
    if (dt <= 0.001f) return;

    dt = Engine::getInstance()->getTime()->getFixedDeltaTime();

    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
    //glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
    float subDt = dt / substeps;
    for (unsigned int i = 0; i < substeps; i++) {
        preSolve(subDt, gravity);
        solve(subDt);
        postSolve(subDt);
    }
}

void TetrahedronSoftBodyMesh::render() {
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
    shader->setVec3("camPos", camera->getTransform()->getGlobalPosition());

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 12);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}