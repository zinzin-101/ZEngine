#pragma once
#include "SoftBodyMesh.h"

class TetrahedronSoftBodyMesh : public SoftBodyMesh {
	private:
		float tetrahedronVertices[4 * 6];
		unsigned int tetrahedronIndices[4 * 3];
		unsigned int volumeIdxOrder[4][3];
		float tetrahedronEdgeId[12];
		float getTetrahedronVolume() const;
		glm::vec3 getPosFromIdx(int idx) const;
		void computeNormal();

		float tempVerticesData[4 * 6]; // and normal

		void preSolve(float dt, glm::vec3 gravity);
		void solve(float dt);
		void postSolve(float dt);
		void solveEdges(float compliance, float dt);
		void solveVolumes(float compliance, float dt);
		void updateMesh();

	public:
		TetrahedronSoftBodyMesh();
		virtual void init() override;
		virtual void update() override;
		virtual void render() override;
};