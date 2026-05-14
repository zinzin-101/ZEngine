#include "SmokeSim.h"
#include "SmokeSimConfig.h"
#include "Engine.h"
#include <glad/glad.h>
#include "render_pipelines/PBRRenderPipeline.h"

bool SmokeSimOperation::isNear(float a, float b) {
	return std::abs(a - b) < 1e-9;
}

void SmokeSimOperation::dispatchComputeShader(int sizeX, int sizeY, int sizeZ) {
	glDispatchCompute((sizeX + 7) / 8, (sizeY + 7) / 8, (sizeZ + 7) / 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

unsigned int SmokeSimOperation::create3DTexture(int sizeX, int sizeY, int sizeZ, GLenum internalFormat, GLenum format, const void* data) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, sizeX, sizeY, sizeZ, 0, format, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}

int SmokeSimOperation::findAbsMaxDimension(glm::vec3 v) {
	v.x = std::abs(v.x);
	v.y = std::abs(v.y);
	v.z = std::abs(v.z);

	int maxDimension = 0;
	float value = v.x;
	if (v.y > value)
	{
		value = v.y;
		maxDimension = 1;
	}
	if (v.z > value)
	{
		value = v.z;
		maxDimension = 2;
	}
	return maxDimension;
}

using namespace SmokeSimConfig;
using namespace SmokeSimOperation;

SmokeSimInfo::SmokeSimInfo(int width, int height, int depth):
	density(DENSITY),
	width(width),
	height(height),
	depth(depth),
	spacing(SPACING),
	obstacleRadius(OBSTACLE_RADIUS)
{}

SmokeSimInfo::SmokeSimInfo(float density, int width, int height, int depth, float spacing, float obstacleRadius):
	density(density),
	width(width),
	height(height),
	depth(depth),
	spacing(spacing),
	obstacleRadius(obstacleRadius)
{}


SmokeSim::SmokeSim(SmokeSimInfo smokeSimInfo):
    density(smokeSimInfo.density),
    sizeX(smokeSimInfo.width + BORDER_SIZE),
	sizeY(smokeSimInfo.height + BORDER_SIZE),
	sizeZ(smokeSimInfo.depth + BORDER_SIZE),
    spacing(smokeSimInfo.spacing),
    obstacleRadius(smokeSimInfo.obstacleRadius),
    obstacleX(0), obstacleY(0), obstacleZ(0),
    integrateShader("compute_shaders/smoke_sim/integrate.comp"),
    vorticityShader("compute_shaders/smoke_sim/vorticity.comp"),
    incompressibilityShader("compute_shaders/smoke_sim/incompressibility.comp"),
    extrapolateShader("compute_shaders/smoke_sim/extrapolate.comp"),
    advectVelocityShader("compute_shaders/smoke_sim/advect_velocity.comp"),
    advectSmokeShader("compute_shaders/smoke_sim/advect_smoke.comp"),
    setObstacleShader("compute_shaders/smoke_sim/set_obstacle.comp"),
    volumeShader("shaders/smoke_volume.vert", "shaders/smoke_volume.frag"),
    velocityTexture(0), newVelocityTexture(0),
    freeSpaceTexture(0), pressureTexture(0),
    smokeTexture(0), newSmokeTexture(0),
    vao(0), vbo(0)
{
	integrateShader.use();
	integrateShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);

	vorticityShader.use();
	vorticityShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);

	incompressibilityShader.use();
	incompressibilityShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);

	extrapolateShader.use();
	extrapolateShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);

	advectVelocityShader.use();
	advectVelocityShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);

	advectSmokeShader.use();
	advectSmokeShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);

	setObstacleShader.use();
	setObstacleShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);

    smokeDirection = DEFAULT_SMOKE_DIRECTION;
	smokeAcceleration = DEFAULT_SMOKE_ACCELERATION;

	simulationDeltaTime = DEFAULT_SIMULATION_DELTA_TIME;

	simulationIterations = DEFAULT_SIMULATION_ITERATIONS;

	int totalCells = sizeX * sizeY * sizeZ;
	smoke = std::vector<float>(totalCells);
	freeSpace = std::vector<float>(totalCells);

	std::fill(smoke.begin(), smoke.end(), 0.0f);
	std::fill(freeSpace.begin(), freeSpace.end(), 1.0f);

	velocityTexture = create3DTexture(sizeX, sizeY, sizeZ, GL_RGBA32F, GL_RGBA, nullptr);
	newVelocityTexture = create3DTexture(sizeX, sizeY, sizeZ, GL_RGBA32F, GL_RGBA, nullptr);
	freeSpaceTexture = create3DTexture(sizeX, sizeY, sizeZ, GL_R32F, GL_RED, freeSpace.data());
	pressureTexture = create3DTexture(sizeX, sizeY, sizeZ, GL_R32F, GL_RED, nullptr);
	smokeTexture = create3DTexture(sizeX, sizeY, sizeZ, GL_R32F, GL_RED, smoke.data());
	newSmokeTexture = create3DTexture(sizeX, sizeY, sizeZ, GL_R32F, GL_RED, nullptr);

	smoke.clear();
	smoke.shrink_to_fit();

	freeSpace.clear();
	freeSpace.shrink_to_fit();

	setInitialObstacle(DEFAULT_SIMULATION_DELTA_TIME, true);

	volumeSlices.resize(MAX_NUMBER_OF_SLICES * 12);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (unsigned int)volumeSlices.size(), nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);

	isTransparent = true;

	simulationTimer = 0.0f;
}

SmokeSim::~SmokeSim() {
	glDeleteTextures(1, &velocityTexture);
	glDeleteTextures(1, &newVelocityTexture);
	glDeleteTextures(1, &freeSpaceTexture);
	glDeleteTextures(1, &pressureTexture);
	glDeleteTextures(1, &smokeTexture);
	glDeleteTextures(1, &newSmokeTexture);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void SmokeSim::update() {
	float deltaTime = Engine::getInstance()->getTime()->getDeltaTime();
	simulationTimer += deltaTime;
	if (simulationTimer < simulationDeltaTime) return;

	const int MAX_STEPS = 5;
	int steps = (int)(simulationTimer / simulationDeltaTime);
	if (steps > MAX_STEPS) {
		steps = MAX_STEPS;
		simulationTimer = 0.0f;
	}
	else {
		simulationTimer -= simulationDeltaTime * (float)steps;
	}

	for (int i = 0; i < steps; i++) {
		float dt = simulationDeltaTime;
		integrate(dt, smokeAcceleration);
		applyVorticity(dt);
		solveIncompressibility(dt, simulationIterations);
		extrapolate();
		advectVelocity(dt);
		advectSmoke(dt);
	}
}

void SmokeSim::render() {
	volumeShader.use();
	Camera* camera = Engine::getInstance()->getCurrentScene()->getCurrentCamera();
	glm::vec3 position = transform->getGlobalPosition();
	glm::vec3 rotation = transform->getGlobalEulerRotation();
	glm::vec3 scale = transform->getGlobalScale();

	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));
	glm::mat4 view = camera->getViewMatrix();

	glm::vec2 screenDimension = Engine::getInstance()->getScreenDimension();
	glm::mat4 projection =
		glm::perspective(glm::radians(camera->fov),
			(float)screenDimension.x / (float)screenDimension.y,
			camera->nearPlane, camera->farPlane);

	GLboolean wasDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean wasCullEnabled = glIsEnabled(GL_CULL_FACE);
	GLint prevCullFace;
	glGetIntegerv(GL_CULL_FACE_MODE, &prevCullFace);

	renderVolume(projection, view, model, *camera);

	wasDepthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	glCullFace(prevCullFace);
	!wasCullEnabled ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
}

void SmokeSim::integrate(float dt, float acceleration) {
	integrateShader.use();
	integrateShader.setFloat("dt", dt);
	integrateShader.setVec3("gravity", acceleration * glm::normalize(smokeDirection));

	glBindImageTexture(0, velocityTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, freeSpaceTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);

	dispatchComputeShader(sizeX, sizeY, sizeZ);
}

void SmokeSim::applyVorticity(float dt) {
	vorticityShader.use();
	vorticityShader.setFloat("dt", dt);
	vorticityShader.setFloat("vorticityStrength", 8.0f);

	glBindImageTexture(0, velocityTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, freeSpaceTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);

	dispatchComputeShader(sizeX, sizeY, sizeZ);
}

void SmokeSim::solveIncompressibility(float dt, int iterations) {
	incompressibilityShader.use();

	// reset all to zero
	float zero = 0.0f;
	glClearTexImage(pressureTexture, 0, GL_RED, GL_FLOAT, &zero);

	float pressureScaling = density * spacing / dt;
	incompressibilityShader.setFloat("pressureScaling", pressureScaling);
	incompressibilityShader.setFloat("overrelaxation", 1.9f);
	incompressibilityShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);

	glBindImageTexture(0, velocityTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, freeSpaceTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
	glBindImageTexture(2, pressureTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

	// using 8-color Gauss-Seidel method
	for (int i = 0; i < iterations; i++) {
		for (int j = 0; j < 8; j++) {
			incompressibilityShader.setInt("pass", j);
			dispatchComputeShader(sizeX, sizeY, sizeZ);
		}
	}
}

void SmokeSim::extrapolate() {
	extrapolateShader.use();
	glBindImageTexture(0, velocityTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	dispatchComputeShader(sizeX, sizeY, sizeZ);
}

void SmokeSim::advectVelocity(float dt) {
	advectVelocityShader.use();
	advectVelocityShader.setFloat("dt", dt);
	advectVelocityShader.setFloat("spacing", spacing);

	glBindImageTexture(0, newVelocityTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, velocityTexture);
	advectVelocityShader.setInt("velocityTexture", 2);

	dispatchComputeShader(sizeX, sizeY, sizeZ);

	std::swap(velocityTexture, newVelocityTexture);
}

void SmokeSim::advectSmoke(float dt) {
	advectSmokeShader.use();
	advectSmokeShader.setFloat("dt", dt);
	advectSmokeShader.setFloat("spacing", spacing);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocityTexture);
	advectSmokeShader.setInt("velocityTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, smokeTexture);
	advectSmokeShader.setInt("smokeTexture", 1);

	glBindImageTexture(0, newSmokeTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
	glBindImageTexture(1, freeSpaceTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);

	dispatchComputeShader(sizeX, sizeY, sizeZ);

	std::swap(smokeTexture, newSmokeTexture);
}

void SmokeSim::setInitialObstacle(float dt, bool reset) {
	setObstacleShader.use();

	setObstacleShader.setFloat("radius", obstacleRadius);
	setObstacleShader.setFloat("spacing", spacing);
	setObstacleShader.setBool("isReset", reset);
	setObstacleShader.setIVec3("gridSize", sizeX, sizeY, sizeZ);
	setObstacleShader.setFloat("smokeColor", 1.0f);
	setObstacleShader.setVec3("vel", glm::vec3(0.0f, -9.81f, 0.0f));

	// Bind 3D Textures
	glBindImageTexture(0, velocityTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, freeSpaceTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(2, smokeTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

	float centerX = (sizeX * spacing) * 0.5f;
	float centerY = obstacleRadius;
	float centerZ = (sizeZ * spacing) * 0.5f;

	obstacleX = centerX;
	obstacleY = obstacleRadius * 2.5f;
	obstacleZ = centerZ;

	setObstacleShader.setVec3("obstaclePos", centerX, centerY, centerZ);
	dispatchComputeShader(sizeX, sizeY, sizeZ);
}

void SmokeSim::renderVolume(glm::mat4 projection, glm::mat4 view, glm::mat4 model, const Camera& camera, int sliceCount) {
	sliceCount = glm::clamp(sliceCount, 16, MAX_NUMBER_OF_SLICES);

	static const glm::vec3 cubeVerts[8] =
	{
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f)
	};

	static const int edgeList[8][12] = {
		{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
		{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is front
		{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is front
		{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is front
		{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is front
		{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is front
		{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is front
		{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is front
	};

	static const int edges[12][2] = {
		{0,1},{1,2},{2,3},{3,0},
		{0,4},{1,5},{2,6},{3,7},
		{4,5},{5,6},{6,7},{7,4}
	};

	glm::vec3 viewDir = glm::normalize(camera.getFoward());
	float maxDist = glm::dot(viewDir, cubeVerts[0]);
	float minDist = maxDist;
	int maxIndex = 0;
	int totalSliceVertices = 0;

	for (int i = 1; i < 8; i++) {
		float dist = glm::dot(viewDir, cubeVerts[i]);
		if (dist > maxDist) {
			maxDist = dist;
			maxIndex = i;
		}
		if (dist < minDist) {
			minDist = dist;
		}
	}

	int maxDimension = findAbsMaxDimension(viewDir);

	minDist -= FLT_EPSILON;
	maxDist += FLT_EPSILON;

	glm::vec3 vecStart[12];
	glm::vec3 vecDir[12];
	float lambda[12];
	float lambdaIncrement[12];
	float denom = 0;

	float planeDist = minDist;
	float planeDistIncrement = (maxDist - minDist) / (float)sliceCount;

	for (int i = 0; i < 12; i++) {
		vecStart[i] = cubeVerts[edges[edgeList[maxIndex][i]][0]];
		vecDir[i] = cubeVerts[edges[edgeList[maxIndex][i]][1]] - vecStart[i];

		denom = glm::dot(vecDir[i], viewDir);

		if (1.0f + denom != 1.0) {
			lambdaIncrement[i] = planeDistIncrement / denom;
			lambda[i] = (planeDist - glm::dot(vecStart[i], viewDir)) / denom;
		}
		else {
			lambda[i] = -1.0;
			lambdaIncrement[i] = 0.0;
		}
	}

	static const int indices[] = { 0,1,2, 0,2,3, 0,3,4, 0,4,5 };

	glm::vec3 intersection[6];
	float dL[12];

	for (int i = sliceCount - 1; i >= 0; i--) {

		for (int e = 0; e < 12; e++)
		{
			dL[e] = lambda[e] + i * lambdaIncrement[e];
		}

		if ((dL[0] >= 0.0) && (dL[0] < 1.0)) {
			intersection[0] = vecStart[0] + dL[0] * vecDir[0];
		}
		else if ((dL[1] >= 0.0) && (dL[1] < 1.0)) {
			intersection[0] = vecStart[1] + dL[1] * vecDir[1];
		}
		else if ((dL[3] >= 0.0) && (dL[3] < 1.0)) {
			intersection[0] = vecStart[3] + dL[3] * vecDir[3];
		}
		else continue;

		if ((dL[2] >= 0.0) && (dL[2] < 1.0)) {
			intersection[1] = vecStart[2] + dL[2] * vecDir[2];
		}
		else if ((dL[0] >= 0.0) && (dL[0] < 1.0)) {
			intersection[1] = vecStart[0] + dL[0] * vecDir[0];
		}
		else if ((dL[1] >= 0.0) && (dL[1] < 1.0)) {
			intersection[1] = vecStart[1] + dL[1] * vecDir[1];
		}
		else {
			intersection[1] = vecStart[3] + dL[3] * vecDir[3];
		}

		if ((dL[4] >= 0.0) && (dL[4] < 1.0)) {
			intersection[2] = vecStart[4] + dL[4] * vecDir[4];
		}
		else if ((dL[5] >= 0.0) && (dL[5] < 1.0)) {
			intersection[2] = vecStart[5] + dL[5] * vecDir[5];
		}
		else {
			intersection[2] = vecStart[7] + dL[7] * vecDir[7];
		}
		if ((dL[6] >= 0.0) && (dL[6] < 1.0)) {
			intersection[3] = vecStart[6] + dL[6] * vecDir[6];
		}
		else if ((dL[4] >= 0.0) && (dL[4] < 1.0)) {
			intersection[3] = vecStart[4] + dL[4] * vecDir[4];
		}
		else if ((dL[5] >= 0.0) && (dL[5] < 1.0)) {
			intersection[3] = vecStart[5] + dL[5] * vecDir[5];
		}
		else {
			intersection[3] = vecStart[7] + dL[7] * vecDir[7];
		}
		if ((dL[8] >= 0.0) && (dL[8] < 1.0)) {
			intersection[4] = vecStart[8] + dL[8] * vecDir[8];
		}
		else if ((dL[9] >= 0.0) && (dL[9] < 1.0)) {
			intersection[4] = vecStart[9] + dL[9] * vecDir[9];
		}
		else {
			intersection[4] = vecStart[11] + dL[11] * vecDir[11];
		}

		if ((dL[10] >= 0.0) && (dL[10] < 1.0)) {
			intersection[5] = vecStart[10] + dL[10] * vecDir[10];
		}
		else if ((dL[8] >= 0.0) && (dL[8] < 1.0)) {
			intersection[5] = vecStart[8] + dL[8] * vecDir[8];
		}
		else if ((dL[9] >= 0.0) && (dL[9] < 1.0)) {
			intersection[5] = vecStart[9] + dL[9] * vecDir[9];
		}
		else {
			intersection[5] = vecStart[11] + dL[11] * vecDir[11];
		}

		for (int k = 0; k < 12; k++) {
			volumeSlices[totalSliceVertices++] = intersection[indices[k]];
		}

	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, totalSliceVertices * sizeof(glm::vec3), &(volumeSlices[0].x));

	glm::mat4 mvp = projection * view * model;
	volumeShader.use();
	volumeShader.setMat4("mvp", mvp);
	volumeShader.setInt("volume", 0);

	PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
	if (pbrPipeline != nullptr) {
		volumeShader.setBool("useDepthOfField", pbrPipeline->isUsingDepthOfField());
		volumeShader.setFloat("farPlane", camera.farPlane);
		volumeShader.setFloat("nearPlane", camera.nearPlane);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, smokeTexture);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, totalSliceVertices);
	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}