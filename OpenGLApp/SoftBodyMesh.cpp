#include "SoftBodyMesh.h"

SoftBodyMesh::SoftBodyMesh() 
	: numberOfParticles{}, numberOfTetrahedrons(0), numberOfEdges(0), 
	vao(0), vbo(0), ebo(0), 
	edgeCompliance(100.0f), volumeCompliance(0.0f), 
	color(0.0f), shader(nullptr),
	groundHeight(0.0f), substeps(10) { }

SoftBodyMesh::~SoftBodyMesh(){
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void SoftBodyMesh::translate(glm::vec3 v) {
	for (unsigned int i = 0; i < numberOfParticles; i++) {
		particlePositions[i] += v;
		previousParticlePositions[i] += v;
	}
}


glm::vec3 SoftBodyMesh::computeFaceNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	glm::vec3 edge1 = p1 - p0;
	glm::vec3 edge2 = p2 - p0;
	return glm::normalize(glm::cross(edge1, edge2));
}