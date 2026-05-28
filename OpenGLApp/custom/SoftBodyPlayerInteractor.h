#pragma once
#include "../GeneralSoftBodyMesh.h"
#include "../Transform.h"

class SoftBodyPlayerInteractor : public Component {
	private:
		GeneralSoftBodyMesh* softbody;

	public:
		Transform* playerTransform;
		float playerRadius;
		float pushForce;

		SoftBodyPlayerInteractor();
		virtual void start();
		virtual void update();
};