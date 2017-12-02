#include "Enemy.h"

namespace game {

	Enemy::Enemy(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : SceneNode(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
	}


	Enemy::~Enemy() {
	}


	glm::quat Enemy::GetAngM(void) const {

		return angm_;
	}


	void Enemy::SetAngM(glm::quat angm) {

		angm_ = angm;
	}


	void Enemy::Update(void) {

		position_.y-=0.3;

		if (position_.y < -13) position_.y = -13;


	}



} // namespace game

