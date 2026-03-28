#pragma once
#include "Scene.h"
#include "TetrahedronSoftBodyMesh.h"

class SoftBodyTestScene : public Scene {
	private:
		SoftBodyMesh* softbodymesh;
		int dragIndex;
		float lastInvMass;
		float dragDepth;

		enum ModelSelect {
			ICOSPHERE,
			ARMADILLO,
			MONKEY,
			COW
		};

		struct SoftBodyConstraints {
			float volumeCompliance;
			float edgeCompliance;
		};

		static ModelSelect modelSelect;
		std::string modelPaths[4];
		SoftBodyConstraints constraints[4];
		std::string icospherePath;
		std::string armadilloPath;
		std::string monkeyPath;
		std::string cowPath;

	protected:
		virtual void loadMeshData() override;
		virtual void setup() override; // for adding objects

	public:
		virtual void processInput() override;
};