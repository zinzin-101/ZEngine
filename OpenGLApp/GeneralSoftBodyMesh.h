#pragma once
#include "SoftBodyMesh.h"

class GeneralSoftBodyMesh : public SoftBodyMesh {
private:
	struct TriangleFace {
		unsigned int indices[3];
		TriangleFace(unsigned int idx0, unsigned int idx1, unsigned int idx2);
		bool operator<(const TriangleFace& other) const;
	};

	float getTetrahedronVolume(int nr);
	void computeRenderingNormal();

	float tempVerticesData[4 * 6]; // and normal
	std::vector<float> defaultVertsPosition;

	void preSolve(float dt, glm::vec3 gravity);
	void solve(float dt);
	void postSolve(float dt);
	void solveEdges(float compliance, float dt);
	void solveVolumes(float compliance, float dt);
	void updateMesh();

public:
	GeneralSoftBodyMesh(std::string filepath);
	virtual void reset() override;
	virtual void init() override;
	virtual void update() override;
	virtual void render() override;
};