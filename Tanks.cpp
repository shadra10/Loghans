#include "Tanks.h"

namespace game {

	Tanks::Tanks(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : Enemy(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		range = 100;
		damage = 15;
	}


	Tanks::~Tanks() {
	}


	glm::quat Tanks::GetAngM(void) const {

		return angm_;
	}


	void Tanks::SetAngM(glm::quat angm) {

		angm_ = angm;
	}


	void Tanks::Update(void) {
		if (!Target->isSafe()) {
			if (shootTimer <= 0) {
				shootTimer = 120;
				shooting = true;
			}
			else
				shooting = false;

			if (!inRange) {
				shooting = false;
				Translate((glm::normalize(Target->GetPosition() - GetPosition()) - glm::vec3(0, 1, 0)) *0.1f);

			}
		}
		else {

			Translate((glm::normalize(patrolPoint - GetPosition()) - glm::vec3(0, 1, 0)) *0.1f);

		}

		float angle = glm::dot(glm::normalize(this->GetForward()), (glm::normalize(-(this->GetPosition() - Target->GetPosition()) * glm::vec3(1, 0, 1))));

		
		glm::quat rotation = glm::angleAxis(angle, glm::vec3(0.0, 0.0, 1.0));
		this->Rotate(rotation);
    	


		//this->Rotate();

		shootTimer--;

		position_.y -= 0.3;

		if (position_.y < -13) position_.y = -13;


	}

	void Tanks::die() {
		draw = false;
	}



} // namespace game

