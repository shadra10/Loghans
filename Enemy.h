#ifndef Enemy_H_
#define Enemy_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "player.h"
#include "scene_node.h"

namespace game {

	// Abstraction of an asteroid
	class Enemy : public SceneNode {

	public:
		// Create asteroid from given resources
		Enemy(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex);

		// Destructor
		~Enemy();


		SceneNode *Target;

		// Get/set attributes specific to asteroids
		glm::quat GetAngM(void) const;
		void SetAngM(glm::quat angm);

		// Update geometry configuration
		void Update(void);

		void die();

		bool isShooting();

		int getDamage();

		int getRange();

		void setTarget(SceneNode* target);
		SceneNode* getTarget(void);

		int getHealth();

		void takeDamage(int);

		void AttackRange();

		void setPatrolPoint(glm::vec3 position);

	protected:
		// Angular momentum of asteroid
		glm::quat angm_;
		int health;
		int damage;
		int shootTimer;
		int range;
		bool shooting;
		bool inRange;
		glm::vec3 patrolPoint;



	}; // class Asteroid

} // namespace game

#endif // ASTEROID_H_
