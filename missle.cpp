#include "missle.h"

namespace game {

	Missle::Missle(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex, glm::vec3 forward) : Bullet(name, geometry, material, tex, 0) {
		radius = 1;
		forwardVelocity = forward;
		lifeSpan = 300;
		damage = 40;
	}


	Missle::~Missle() {
	}


	void Missle::SetVelocity(glm::vec3 velocity) {
	
		velocity_ = velocity;
	}

	void Missle::die() {
		draw = false;

	}


	void Missle::Update(void) {

		lifeSpan--;
		if (lifeSpan <= 0) die();
		//position_ += velocity_;
		velocity_ += forwardVelocity * 0.005f;
		Translate(velocity_);
	}

} // namespace game
