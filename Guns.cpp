#include "Guns.h"

namespace game {

	Guns::Guns(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : Enemy(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		damage = 10;
	}


	Guns::~Guns() {
	}


	glm::quat Guns::GetAngM(void) const {

		return angm_;
	}


	void Guns::SetAngM(glm::quat angm) {

		angm_ = angm;
	}


	void Guns::Update(void) {

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

	void Guns::die() {
		draw = false;
	}



} // namespace game

