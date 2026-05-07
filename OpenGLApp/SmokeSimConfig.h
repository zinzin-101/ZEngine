#pragma once
namespace SmokeSimConfig {
	const int BORDER_SIZE = 2;
	const int BORDER_OFFSET = BORDER_SIZE / 2;
	const float OVERRELAXATION = 1.9f;
	const float DENSITY = 1000.0f;
	const float SPACING = 3.0f;
	const float OBSTACLE_RADIUS = 16.0f;
	const int MAX_NUMBER_OF_SLICES = 512;
	const float DEFAULT_SMOKE_ACCELERATION = 9.81f;
	const glm::vec3 DEFAULT_SMOKE_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
	const float DEFAULT_SIMULATION_DELTA_TIME = 1.0f / 60.0f;
	const int DEFAULT_SIMULATION_ITERATIONS = 1;
}