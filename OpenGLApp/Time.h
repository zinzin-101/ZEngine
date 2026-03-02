#pragma once

class Time {
	private:
		float lastTimeElapsed;
		float deltaTime;

	public:
		float timeScale;
		Time();
		void update();
		float getDeltaTime() const;
		float getRealDeltaTime() const;
};