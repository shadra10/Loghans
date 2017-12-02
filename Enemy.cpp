#include "Enemy.h"

namespace game {

	Enemy::Enemy(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : SceneNode(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		shootTimer = 120;
		health = 100;
		damage = 10;
		shooting = false;
	}


	Enemy::~Enemy() {
	}


	glm::quat Enemy::GetAngM(void) const {

		return angm_;
	}


	void Enemy::SetAngM(glm::quat angm) {

		angm_ = angm;
	}

	bool Enemy::isShooting(void) {

		return shooting;
	}


	int Enemy::getDamage(void) {

		return damage;
	}


	void Enemy::Update(void) {


		if (shootTimer <= 0) {
			shootTimer = 120;
			shooting = true;
		}
		else
			shooting = false;

		shootTimer--;

		position_.y-=0.3;

		if (position_.y < -13) position_.y = -13;


	}

	void Enemy::die() {
		draw = false;
	}



} // namespace game

