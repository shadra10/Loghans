#include "bullet.h"

namespace game {

	Bullet::Bullet(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex, int type) : SceneNode(name, geometry, material, tex) {
		radius = 3;
		damage = 25;
		lifeSpan = 500;
		Type = type;
	}


	Bullet::~Bullet() {
	}


	void Bullet::SetVelocity(glm::vec3 velocity) {

		velocity_ = velocity;
	}

	void Bullet::die() {
		draw = false;
	}

	void Bullet::SetDamage(int newDamage) {

		damage = newDamage;
	}

	int Bullet::GetDamage(void) {
		return damage;
	}

	int Bullet::GetType(void) {

		return Type;

	}

	int Bullet::GetLifeSpan(void) {

		return lifeSpan;

	}

	void Bullet::Update(void) {

		lifeSpan--;
		//position_ += velocity_;
		Translate(velocity_);
	}

} // namespace game
