#pragma once
#include "../Component.h"
#include "../Camera.h"
#include "../SkeletalAnimator.h"
#include "../SkeletalModel.h"

class PlayerController : public Component {
	private:
		enum PlayerState {
			IDLE,
			JOG
		};

		PlayerState currentState;
		float moveSpeed;
		SkeletalAnimator* animator;
		SkeletalModel* model;

	public:
		Camera* camera;
		PlayerController();
		virtual void start() override;
		virtual void update() override;
		void move(glm::vec2 input);
};