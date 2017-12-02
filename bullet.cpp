#include "bullet.h"

namespace game {

	Bullet::Bullet(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex) : SceneNode(name, geometry, material, tex) {
	}


	Bullet::~Bullet() {
	}


	void Bullet::SetVelocity(glm::vec3 velocity) {
	
		velocity_ = velocity;
	}

	void Bullet::Update(void) {

		//position_ += velocity_;
		Translate(velocity_);
	}

} // namespace game
