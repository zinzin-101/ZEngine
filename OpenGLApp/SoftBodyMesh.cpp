#include "SoftBodyMesh.h"

void SoftBodyMesh::translate(glm::vec3 v) {
	for (int i = 0; i < numberOfParticles; i++) {
		particlePositions[i] += v;
		previousParticlePositions[i] += v;
	}
}


glm::vec3 SoftBodyMesh::computeFaceNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	glm::vec3 edge1 = p1 - p0;
	glm::vec3 edge2 = p2 - p0;
	return glm::normalize(glm::cross(edge1, edge2));
}