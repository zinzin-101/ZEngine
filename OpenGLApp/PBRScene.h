#pragma once
#include "Scene.h"

class Model;
class PlayerController;
class Camera;

class PBRScene : public Scene {
	private:
		float depthPercentage;
		Model* modelPtr;
		PlayerController* player;
		Camera* playerCam;
		Camera* freeCam;

	protected:
		virtual void loadMeshData() override;
		virtual void setup() override;

	public:
		virtual ~PBRScene() override;
		virtual void processInput() override;
};