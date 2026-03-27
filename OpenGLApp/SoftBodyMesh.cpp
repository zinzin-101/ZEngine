#include "SoftBodyMesh.h"

using namespace SoftBodyVectorOperation;

SoftBodyMesh::SoftBodyMesh() 
	: numberOfParticles{}, numberOfTetrahedrons(0), numberOfEdges(0), 
	temp{}, grads{},
	vao(0), vbo(0), 
	edgeCompliance(100.0f), volumeCompliance(0.0f), 
	color(0.0f), shader(nullptr),
	groundHeight(0.0f), substeps(10), pauseSimulation(false) { }

SoftBodyMesh::~SoftBodyMesh(){
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	//glDeleteBuffers(1, &ebo);
}

glm::vec3 SoftBodyMesh::computeFaceNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	glm::vec3 edge1 = p1 - p0;
	glm::vec3 edge2 = p2 - p0;
	return glm::normalize(glm::cross(edge1, edge2));
}

void SoftBodyMesh::setParticlePosition(int index, glm::vec3 position) {
	float v[3] = { position.x, position.y, position.z };
	vecCopy(particlePositions.data(), index, v, 0);
	vecCopy(previousParticlePositions.data(), index, v, 0);
	vecSetZero(particleVelocities.data(), index);
}

void SoftBodyMesh::setInverseMass(int index, float mass) {
	invMasses[index] = mass;
}

float SoftBodyMesh::getInverseMass(int index) const {
	return invMasses.at(index);
}

const std::vector<float>& SoftBodyMesh::getParticlePositionsData() const {
	return particlePositions;
}

namespace SoftBodyVectorOperation {
	void vecSetZero(float* a, int anr){
		anr *= 3;
		a[anr++] = 0.0;
		a[anr++] = 0.0;
		a[anr] = 0.0;
	}

	void vecScale(float* a, int anr, float scale){
		anr *= 3;
		a[anr++] *= scale;
		a[anr++] *= scale;
		a[anr] *= scale;
	}

	void vecCopy(float* a, int anr, float* b, int bnr){
		anr *= 3; bnr *= 3;
		a[anr++] = b[bnr++];
		a[anr++] = b[bnr++];
		a[anr] = b[bnr];
	}

	void vecAdd(float* a, int anr, float* b, int bnr, float scale){
		anr *= 3; bnr *= 3;
		a[anr++] += b[bnr++] * scale;
		a[anr++] += b[bnr++] * scale;
		a[anr] += b[bnr] * scale;
	}

	void vecSetDiff(float* dst, int dnr, float* a, int anr, float* b, int bnr, float scale){
		dnr *= 3; anr *= 3; bnr *= 3;
		dst[dnr++] = (a[anr++] - b[bnr++]) * scale;
		dst[dnr++] = (a[anr++] - b[bnr++]) * scale;
		dst[dnr] = (a[anr] - b[bnr]) * scale;
	}

	float vecLengthSquared(float* a, int anr){
		anr *= 3;
		float a0 = a[anr], a1 = a[anr + 1], a2 = a[anr + 2];
		return a0 * a0 + a1 * a1 + a2 * a2;
	}

	float vecDistSquared(float* a, int anr, float* b, int bnr){
		anr *= 3; bnr *= 3;
		float a0 = a[anr] - b[bnr], a1 = a[anr + 1] - b[bnr + 1], a2 = a[anr + 2] - b[bnr + 2];
		return a0 * a0 + a1 * a1 + a2 * a2;
	}

	float vecDot(float* a, int anr, float* b, int bnr){
		anr *= 3; bnr *= 3;
		return a[anr] * b[bnr] + a[anr + 1] * b[bnr + 1] + a[anr + 2] * b[bnr + 2];
	}

	void vecSetCross(float* a, int anr, float* b, int bnr, float* c, int cnr){
		anr *= 3; bnr *= 3; cnr *= 3;
		a[anr++] = b[bnr + 1] * c[cnr + 2] - b[bnr + 2] * c[cnr + 1];
		a[anr++] = b[bnr + 2] * c[cnr + 0] - b[bnr + 0] * c[cnr + 2];
		a[anr] = b[bnr + 0] * c[cnr + 1] - b[bnr + 1] * c[cnr + 0];
	}

	bool isNear(float a, float b) {
		return std::fabs(a - b) < 1e-9;
	}
}