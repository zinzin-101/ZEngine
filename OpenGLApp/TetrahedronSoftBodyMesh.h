#pragma once
#include "SoftBodyMesh.h"

class TetrahedronSoftBodyMesh : public SoftBodyMesh {
	private:
		float getTetrahedronVolume(int nr);
		void computeRenderingNormal();

		float tempVerticesData[4 * 6]; // and normal

		void preSolve(float dt, glm::vec3 gravity);
		void solve(float dt);
		void postSolve(float dt);
		void solveEdges(float compliance, float dt);
		void solveVolumes(float compliance, float dt);
		void updateMesh();

	public:
		TetrahedronSoftBodyMesh();
		virtual void reset() override;
		virtual void init() override;
		virtual void update() override;
		virtual void render() override;
};