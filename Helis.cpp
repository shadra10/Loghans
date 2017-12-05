#include "Helis.h"

namespace game {

	Helis::Helis(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : Enemy(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		damage = 10;
		range = 100;
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
		if (!Target->isSafe()) {
			if (shootTimer <= 0) {
				shootTimer = 120;
				shooting = true;
			}
			else
				shooting = false;

			if (!inRange) {
				shooting = false;
				Translate(glm::normalize(Target->GetPosition() - GetPosition()) *0.5f);
			}
			else
				Translate(glm::normalize(glm::cross(glm::normalize(Target->GetPosition() - GetPosition()), glm::vec3(0.0, 1.0, 0.0))) * 0.2f);

			shootTimer--;
		}
		else {
		
			Translate(glm::normalize(patrolPoint - GetPosition()) *0.5f);
		
		}
		float angle = glm::dot(glm::normalize(this->GetForward()), (glm::normalize(-(this->GetPosition() - Target->GetPosition()) * glm::vec3(1, 0, 1))));


		glm::quat rotation = glm::angleAxis(angle, glm::vec3(0.0, 0.0, 1.0));
		this->Rotate(rotation);

		glm::vec3 h_diff = Target->GetPosition() - this->GetPosition();
		position_.y += h_diff.y;

		if (position_.y < -13) position_.y = -13;


	}



	void Helis::die() {
		draw = false;
	}

	void Helis::AttackRange() {

		glm::vec3 s = Target->GetPosition() - GetPosition(); // vector between the centers of each sphere


		float r = Target->GetRadius() + getRange();

		double c = glm::dot(s, s) - r*r; // if negative, they overlap
		if (c < 0.0) // if true, they already overlap
		{
			inRange = true;
		}
		else
			inRange = false;
	}



} // namespace game

