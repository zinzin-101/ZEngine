#include "Time.h"
#include "GLFW/glfw3.h"

Time::Time(): timeScale(1.0f), lastTimeElapsed(0.0f), deltaTime(0.0f) {}

void Time::updateTime(){
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

void Time::setTimeScale(float timeScale) {
    this->timeScale = timeScale;
}