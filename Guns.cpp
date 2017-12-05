#include "Guns.h"
#include <iostream>
namespace game {

	Guns::Guns(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : Enemy(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		damage = 1;
		range = 250;
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
			shootTimer = 60;
			shooting = true;
		}
		else
			shooting = false;

		if (!inRange || Target->isSafe()) {
			shooting = false;
			
		}

		shootTimer--;


		float angle = glm::dot(glm::normalize(this->GetForward()), (glm::normalize(-(this->GetPosition() - Target->GetPosition()) * glm::vec3(1, 0, 1))));

		glm::quat rotation = glm::angleAxis(angle, glm::vec3(0.0, 0.0, 1.0));
		this->Rotate(rotation);
	


	}

	void Guns::die() {
		draw = false;
	}



} // namespace game

