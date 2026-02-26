#pragma once

class Time {
	private:
		float timeScale;
		float lastTimeElapsed;
		float deltaTime;

	public:
		Time();
		void update();
		float getDeltaTime() const;
		float getRealDeltaTime() const;
		void setTimeScale(float timeScale);
};