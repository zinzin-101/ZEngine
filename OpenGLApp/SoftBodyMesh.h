#pragma once
#include "Component.h"
#include "shader.h"
#include <vector>

class SoftBodyMesh : public Component {
	protected:
		int numberOfParticles;
		int numberOfTetrahedrons;
		int numberOfEdges;
		std::vector<float> particlePositions;
		std::vector<float> renderVertices;
		std::vector<float> previousParticlePositions;
		std::vector<int> tetrahedronIndices;
		std::vector<int> edgeIndices;
		std::vector<int> volumeIndicesOrder;
		std::vector<float> particleVelocities;
		std::vector<float> restVolumes;
		std::vector<float> edgeLengths;
		std::vector<float> invMasses;

		float temp[4 * 3];
		float grads[4 * 3];

		unsigned int vao;
		unsigned int vbo;
		//unsigned int ebo;

		glm::vec3 computeFaceNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);

	public:
		SoftBodyMesh();
		~SoftBodyMesh();
		virtual void reset() = 0;
		float edgeCompliance;
		float volumeCompliance;
		glm::vec3 color;
		Shader* shader;
		float groundHeight; // temp
		unsigned int substeps;
		bool pauseSimulation;

		void setParticlePosition(int index, glm::vec3 position);
		void setInverseMass(int index, float mass);
		float getInverseMass(int index) const;
		const std::vector<float>& getParticlePositionsData() const;
};

namespace SoftBodyVectorOperation {
	void vecSetZero(float* a, int anr);
	void vecScale(float* a, int anr, float scale);
	void vecCopy(float* a, int anr, float* b, int bnr);
	void vecAdd(float* a, int anr, float* b, int bnr, float scale = 1.0f);
	void vecSetDiff(float* dst, int dnr, float* a, int anr, float* b, int bnr, float scale = 1.0f);
	float vecLengthSquared(float* a, int anr);
	float vecDistSquared(float* a, int anr, float* b, int bnr);
	float vecDot(float* a, int anr, float* b, int bnr);
	void vecSetCross(float* a, int anr, float* b, int bnr, float* c, int cnr);
	bool isNear(float a, float b);
}