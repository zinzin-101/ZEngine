#pragma once
#include "Scene.h"

class TestScene : public Scene {
	public:
		virtual void setup() override; // for adding objects
		virtual void init() override; // for component setup
		virtual void start() override; // for object's custom behavior
		virtual void update() override;
		virtual void processInput() override;
};