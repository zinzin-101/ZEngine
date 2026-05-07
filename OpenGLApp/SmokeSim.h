#pragma once
#include "Camera.h"
#include "Component.h"
#include "ComputeShader.h"
#include <vector>
#include <shader.h>

struct SmokeSimInfo {
	SmokeSimInfo(int width, int height, int depth);
	SmokeSimInfo(float density, int width, int height, int depth, float spacing, float obstacleRadius);
	float density;
	int width;
	int height;
	int depth;
	float spacing;
	float obstacleRadius;
};

class SmokeSim : public Component {
	private:
		float density;
		int sizeX;
		int sizeY;
		int sizeZ;
		float spacing;
		float obstacleRadius;
		float obstacleX, obstacleY, obstacleZ;
		std::vector<float> freeSpace;
		std::vector<float> smoke;

		// compute shader
		ComputeShader integrateShader;
		ComputeShader vorticityShader;
		ComputeShader incompressibilityShader;
		ComputeShader extrapolateShader;
		ComputeShader advectVelocityShader;
		ComputeShader advectSmokeShader;
		ComputeShader setObstacleShader;

		// texture for compute shader
		unsigned int velocityTexture;
		unsigned int newVelocityTexture;
		unsigned int freeSpaceTexture;
		unsigned int pressureTexture;
		unsigned int smokeTexture;
		unsigned int newSmokeTexture;

		// simulation
		void integrate(float dt, float acceleration);
		void applyVorticity(float dt);
		void solveIncompressibility(float dt, int iterations);
		void extrapolate();
		void advectVelocity(float dt);
		void advectSmoke(float dt);
		void setInitialObstacle(float dt, bool reset);

		// volume rendering
		std::vector<glm::vec3> volumeSlices;
		unsigned int vao;
		unsigned int vbo;
		Shader volumeShader;
		void renderVolume(glm::mat4 projection, glm::mat4 view, glm::mat4 model, const Camera& camera, int sliceCount = 128);

	public:
		glm::vec3 smokeDirection;
		float smokeAcceleration;
		float simulationDeltaTime;
		int simulationIterations;

		SmokeSim(SmokeSimInfo smokeSimInfo);
		~SmokeSim();
		virtual void update() override;
		virtual void render() override;
};

namespace SmokeSimOperation {
	bool isNear(float a, float b);
	void dispatchComputeShader(int sizeX, int sizeY, int sizeZ);
	unsigned int create3DTexture(int sizeX, int sizeY, int sizeZ, GLenum internalFormat, GLenum format, const void* data);
	int findAbsMaxDimension(glm::vec3 v);
}