#pragma once
#include "Component.h"
#include "shader.h"
#include <vector>

class SoftBodyMesh : public Component {
	protected:
		unsigned int numberOfParticles;
		unsigned int numberOfTetrahedrons;
		unsigned int numberOfEdges;
		std::vector<glm::vec3> particlePositions;
		std::vector<glm::vec3> tetrahedronNormals;
		std::vector<glm::vec3> previousParticlePositions;
		std::vector<glm::vec3> particleVelocities;
		std::vector<float> restVolumes;
		std::vector<float> edgeLengths;
		std::vector<float> invMasses;

		glm::vec3 tempVectors[4];
		glm::vec3 gradientVectors[4];

		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;

		void translate(glm::vec3 v);
		glm::vec3 computeFaceNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);

	public:
		float edgeCompliance;
		float volumeCompliance;
		glm::vec3 color;
		Shader* shader;

		float groundHeight; // temp
};