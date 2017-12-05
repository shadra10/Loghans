#include "Enemy.h"

namespace game {

	Enemy::Enemy(const std::string name, const Resource *geometry, const Resource *material, const Resource * tex) : SceneNode(name, geometry, material, tex) {
		speed = 0.5;
		radius = 1;
		range = 1;
		shootTimer = 120;
		health = 100;
		damage = 10;
		shooting = false;
		inRange = false;
		Target = NULL;
	}


	Enemy::~Enemy() {
	}


	glm::quat Enemy::GetAngM(void) const {

		return angm_;
	}


	void Enemy::SetAngM(glm::quat angm) {

		angm_ = angm;
	}

	bool Enemy::isShooting(void) {

		return shooting;
	}


	int Enemy::getDamage(void) {

		return damage;
	}

	int Enemy::getRange(void) {

		return range;
	}



	void Enemy::Update(void) {

		
			if (shootTimer <= 0) {
				shootTimer = 120;
				shooting = true;
			}
			else
				shooting = false;
		
		if(!inRange) {
			shooting = false;
			Translate(Target->GetPosition() - GetPosition());
		}
		
		shootTimer--;

		position_.y -= 0.3;

		if (position_.y < -13) position_.y = -13;


	}


	void Enemy::setTarget(SceneNode *target) {
		Target = target;
	}

	SceneNode* Enemy::getTarget(void) {
		return Target;
	}




	void Enemy::takeDamage(int damageTaken) {

		health -= damageTaken;
	}

	int Enemy::getHealth() {
	
		return health;
	}

	void Enemy::die() {
		draw = false;
	}

	void Enemy::setPatrolPoint(glm::vec3 point) {
		patrolPoint = point;
	
	}


	void Enemy::AttackRange() {

		glm::vec3 s = Target->GetPosition() - GetPosition(); // vector between the centers of each sphere
					
															 //std::vector<double> v = node1->velocity - node2->vel; // relative velocity between spheres
		s.y = 0;
		
		float r =Target->GetRadius() + getRange();

		double c = glm::dot(s, s) - r*r; // if negative, they overlap
		if (c < 0.0) // if true, they already overlap
		{
			inRange = true;
		}
		else
			inRange = false;
	}




} // namespace game

