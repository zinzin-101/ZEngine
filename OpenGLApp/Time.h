#pragma once

class Time {
	private:
		float timeScale;
		float lastTimeElapsed;
		float deltaTime;

	public:
		Time();
		void updateTime();
		float getDeltaTime() const;
		float getRealDeltaTime() const;
		void setTimeScale(float timeScale);
};