#include "Player.h"

namespace game {

	Player::Player(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : SceneNode(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		draw = true;

		safe = false;
		health = 200;

	}


	Player::~Player() {
	}


	glm::quat Player::GetAngM(void) const {

		return angm_;
	}


	void Player::SetAngM(glm::quat angm) {

		angm_ = angm;
	}


	void Player::takeDamage(int damageTaken) {

		health -= damageTaken;
	}

	float Player::getHealth() {

		return health;
	}


	void Player::Update(void) {

		
		if (position_.y < -13) position_.y = -13;


	}

	void Player::setDraw(bool newDraw) {
		draw = newDraw;
	}

	


} // namespace game

