#include "BaeHawk.h"

namespace game {

	BaeHawk::BaeHawk(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex, SceneNode *meter) : SceneNode(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		draw = true;

		health = 200;

		Affection = meter;

	}


	BaeHawk::~BaeHawk() {
	}


	glm::quat BaeHawk::GetAngM(void) const {

		return angm_;
	}


	void BaeHawk::SetAngM(glm::quat angm) {

		angm_ = angm;
	}

	void BaeHawk::setPlayer(Player * pla) {

		player = pla;
	}

	void BaeHawk::takeDamage(int damageTaken) {

		health -= damageTaken;
	}

	int BaeHawk::getHealth() {

		return health;
	}


	void BaeHawk::Update(void) {


		float angle = glm::dot(glm::normalize(this->GetForward()), (glm::normalize(-(this->GetPosition() - player->GetPosition()) * glm::vec3(1, 0, 1))));


		glm::quat rotation = glm::angleAxis(angle, glm::vec3(0.0, 0.0, 1.0));
		this->Rotate(rotation);
		
		if (position_.y < -13) position_.y = -13;

		

	}

	void BaeHawk::setDraw(bool newDraw) {
		draw = newDraw;
	}

	SceneNode* BaeHawk::getAffection() {
		return Affection;
	}


} // namespace game

