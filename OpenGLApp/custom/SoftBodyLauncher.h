#pragma once
#include "../GeneralSoftBodyMesh.h"

class SoftBodyLauncher : public Component {
	private:
		GeneralSoftBodyMesh* softbody;

	public:
		bool useUniformLaunch;
		float upwardSpeed;

		SoftBodyLauncher();
		virtual void start() override;
		virtual void update() override;
};