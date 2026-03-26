#include "GeneralSoftBodyMesh.h"
#include "Engine.h"
#include <tiny_obj_loader.h>
#include <glad/glad.h>
#include <algorithm>
#include <map>

using namespace SoftBodyVectorOperation;

GeneralSoftBodyMesh::TriangleFace::TriangleFace(unsigned int idx0, unsigned int idx1, unsigned int idx2) {
    indices[0] = idx0;
    indices[1] = idx1;
    indices[2] = idx2;

    std::sort(indices, indices + 3);
}

bool GeneralSoftBodyMesh::TriangleFace::operator<(const TriangleFace& other) const {
    if (indices[0] != other.indices[0]) return indices[0] < other.indices[0];
    if (indices[1] != other.indices[1]) return indices[1] < other.indices[1];
    return indices[2] < other.indices[2];
}

GeneralSoftBodyMesh::GeneralSoftBodyMesh(std::string filepath) : tempVerticesData{ 0.0f } {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str(), nullptr, false)) {
        throw std::runtime_error("Softbody mesh constructor failed to load: " + err);
    }

    numberOfParticles = (int)attrib.vertices.size() / 3;

    for (int i = 0; i < numberOfParticles; i++) {
        particlePositions.emplace_back(attrib.vertices[3 * i + 0]);
        particlePositions.emplace_back(attrib.vertices[3 * i + 1]);
        particlePositions.emplace_back(attrib.vertices[3 * i + 2]);

        previousParticlePositions.emplace_back(attrib.vertices[3 * i + 0]);
        previousParticlePositions.emplace_back(attrib.vertices[3 * i + 1]);
        previousParticlePositions.emplace_back(attrib.vertices[3 * i + 2]);

        defaultVertsPosition.emplace_back(attrib.vertices[3 * i + 0]);
        defaultVertsPosition.emplace_back(attrib.vertices[3 * i + 1]);
        defaultVertsPosition.emplace_back(attrib.vertices[3 * i + 2]);
    }

    for (const tinyobj::shape_t& shape : shapes) {
        int index_offset = 0;
        int n = (int)shape.mesh.num_face_vertices.size();
        for (int f = 0; f < n; f++) {
            int fv = shape.mesh.num_face_vertices[f];

            if (fv == 4) {
                for (int v = 0; v < 4; v++) { // v < fv and fv should be 4
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    tetrahedronIndices.push_back(idx.vertex_index);
                }
            }

            index_offset += fv;
        }
    }
    numberOfTetrahedrons = (int)tetrahedronIndices.size() / 4;

    unsigned int edgeIdx[] = {
        0, 1,
        0, 2,
        0, 3,
        1, 2,
        1, 3,
        2, 3
    };

    for (int i = 0; i < numberOfTetrahedrons; i++) {
        int idx[4];
        idx[0] = tetrahedronIndices[4 * i + 0];
        idx[1] = tetrahedronIndices[4 * i + 1];
        idx[2] = tetrahedronIndices[4 * i + 2];
        idx[3] = tetrahedronIndices[4 * i + 3];

        for (int j = 0; j < 6; j++) {
            edgeIndices.emplace_back(idx[edgeIdx[2 * j + 0]]);
            edgeIndices.emplace_back(idx[edgeIdx[2 * j + 1]]);
        }
    }

    unsigned int volumeIdxOrder[] = {
        1, 3, 2,
        0, 2, 3,
        0, 3, 1,
        0, 1, 2
    };

    std::map<TriangleFace, int> faceCount; // count number of faces that are used by tetrahedron
    for (int i = 0; i < numberOfTetrahedrons; i++) {
        int tet[4] = { 
            tetrahedronIndices[4 * i],
            tetrahedronIndices[4 * i + 1],
            tetrahedronIndices[4 * i + 2], 
            tetrahedronIndices[4 * i + 3] 
        };

        for (int f = 0; f < 4; f++) {
            int idx0 = tet[volumeIdxOrder[3 * f + 0]];
            int idx1 = tet[volumeIdxOrder[3 * f + 1]];
            int idx2 = tet[volumeIdxOrder[3 * f + 2]];
            faceCount[TriangleFace(idx0, idx1, idx2)]++;
        }
    }

    for (int i = 0; i < numberOfTetrahedrons; i++) {
        int tet[4] = {
            tetrahedronIndices[4 * i],
            tetrahedronIndices[4 * i + 1],
            tetrahedronIndices[4 * i + 2],
            tetrahedronIndices[4 * i + 3]
        };


        for (int f = 0; f < 4; f++) { // For each of the 4 faces
            int idx0 = tet[volumeIdxOrder[3 * f + 0]];
            int idx1 = tet[volumeIdxOrder[3 * f + 1]];
            int idx2 = tet[volumeIdxOrder[3 * f + 2]];

            // if a face is only connected to one tetrahedron then it's on the surface
            if (faceCount.at(TriangleFace(idx0, idx1, idx2)) == 1) {
                volumeIndicesOrder.push_back(idx0);
                volumeIndicesOrder.push_back(idx1);
                volumeIndicesOrder.push_back(idx2);

                glm::vec3 p0(particlePositions[3 * idx0], particlePositions[3 * idx0 + 1], particlePositions[3 * idx0 + 2]);
                glm::vec3 p1(particlePositions[3 * idx1], particlePositions[3 * idx1 + 1], particlePositions[3 * idx1 + 2]);
                glm::vec3 p2(particlePositions[3 * idx2], particlePositions[3 * idx2 + 1], particlePositions[3 * idx2 + 2]);

                glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));

                float vData[3][6] = {
                    {p0.x, p0.y, p0.z, normal.x, normal.y, normal.z},
                    {p1.x, p1.y, p1.z, normal.x, normal.y, normal.z},
                    {p2.x, p2.y, p2.z, normal.x, normal.y, normal.z}
                };

                for (int v = 0; v < 3; v++){
                    for (int d = 0; d < 6; d++) {
                        renderVertices.push_back(vData[v][d]);
                    }
                }
            }

        }
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

void GeneralSoftBodyMesh::reset() {
    int n = (int)particlePositions.size() / 3;
    for (int i = 0; i < n; i++) {
        particlePositions[3 * i + 0] = defaultVertsPosition[3 * i + 0];
        particlePositions[3 * i + 1] = defaultVertsPosition[3 * i + 1];
        particlePositions[3 * i + 2] = defaultVertsPosition[3 * i + 2];

        previousParticlePositions[3 * i + 0] = defaultVertsPosition[3 * i + 0];
        previousParticlePositions[3 * i + 1] = defaultVertsPosition[3 * i + 1];
        previousParticlePositions[3 * i + 2] = defaultVertsPosition[3 * i + 2];

        particleVelocities[3 * i + 0] = 0.0f;
        particleVelocities[3 * i + 1] = 0.0f;
        particleVelocities[3 * i + 2] = 0.0f;
    }
}

float GeneralSoftBodyMesh::getTetrahedronVolume(int nr) {
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

void GeneralSoftBodyMesh::preSolve(float dt, glm::vec3 gravity) {
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

void GeneralSoftBodyMesh::solve(float dt) {
    solveEdges(edgeCompliance, dt);
    solveVolumes(volumeCompliance, dt);
}

void GeneralSoftBodyMesh::postSolve(float dt) {
    for (int i = 0; i < numberOfParticles; i++) {
        if (isNear(invMasses[i], 0.0f)) continue;
        vecSetDiff(particleVelocities.data(), i, particlePositions.data(), i, previousParticlePositions.data(), i, 1.0f / dt);
    }
    updateMesh();
}

void GeneralSoftBodyMesh::solveEdges(float compliance, float dt) {
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

void GeneralSoftBodyMesh::solveVolumes(float compliance, float dt) {
    float alpha = compliance / (dt * dt);

    unsigned int volIdx[] = { 1, 3, 2, 0, 2, 3, 0, 3, 1, 0, 1, 2 };

    for (int i = 0; i < numberOfTetrahedrons; i++) {
        float w = 0.0f;
        for (int j = 0; j < 4; j++) {
            int idx0 = tetrahedronIndices[4 * i + volIdx[3 * j + 0]];
            int idx1 = tetrahedronIndices[4 * i + volIdx[3 * j + 1]];
            int idx2 = tetrahedronIndices[4 * i + volIdx[3 * j + 2]];

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

void GeneralSoftBodyMesh::computeRenderingNormal() {
    int totalVertices = (int)renderVertices.size() / 6;
    int totalFaces = totalVertices / 3;

    for (int face = 0; face < totalFaces; face++) {
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

void GeneralSoftBodyMesh::updateMesh() {
    //glm::vec3 position = transform->getGlobalPosition();
    for (int i = 0; i < volumeIndicesOrder.size(); i++) {
        int index = volumeIndicesOrder[i];
        renderVertices[i * 6 + 0] = particlePositions[index * 3 + 0];// + position.x;
        renderVertices[i * 6 + 1] = particlePositions[index * 3 + 1];// + position.y;
        renderVertices[i * 6 + 2] = particlePositions[index * 3 + 2];// + position.z;
    }
    computeRenderingNormal();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderVertices.size() * sizeof(float), renderVertices.data());
}

void GeneralSoftBodyMesh::init() {
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

void GeneralSoftBodyMesh::update() {
    float dt = Engine::getInstance()->getTime()->getDeltaTime();
    if (dt <= 0.001f || pauseSimulation) return;

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

void GeneralSoftBodyMesh::render() {
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

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, (unsigned int)renderVertices.size() / 6);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
}