#include "Time.h"
#include "Engine.h"
#include "GLFW/glfw3.h"

Time::Time(): timeScale(1.0f), lastTimeElapsed(0.0f), deltaTime(0.0f) {}

void Time::update(){
    float currentTimeElapsed = static_cast<float>(glfwGetTime());
    deltaTime = currentTimeElapsed - lastTimeElapsed;
    lastTimeElapsed = currentTimeElapsed;
}

float Time::getDeltaTime() const{
    return deltaTime * timeScale;
}

float Time::getRealDeltaTime() const {
    return deltaTime;
}

float Time::getFixedDeltaTime() const {
    return EngineConfig::FIXED_DT;
}