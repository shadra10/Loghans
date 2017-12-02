#include "Helis.h"

namespace game {

	Helis::Helis(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : Enemy(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		damage = 15;
	}


	Helis::~Helis() {
	}


	glm::quat Helis::GetAngM(void) const {

		return angm_;
	}


	void Helis::SetAngM(glm::quat angm) {

		angm_ = angm;
	}


	void Helis::Update(void) {

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

	void Helis::die() {
		draw = false;
	}



} // namespace game

