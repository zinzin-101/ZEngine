#pragma once
#include "Scene.h"

class TestScene : public Scene {
	virtual void update() override;
	virtual void processInput() override;
};