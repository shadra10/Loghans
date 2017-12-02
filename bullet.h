#ifndef BULLET_H_
#define BULLET_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include <iostream>

#include "resource.h"
#include "scene_node.h"

namespace game {

	// Abstraction of an Bullet
	class Bullet : public SceneNode {

	public:
		// Create asteroid from given resources
		Bullet(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex, int type);

		// Destructor
		~Bullet();

		void SetVelocity(glm::vec3 velocity);

		void die();

		void SetDamage(int damage);

		int GetDamage(void);
		int GetType(void);
		int GetLifeSpan(void);

		// Update geometry configuration
		void Update(void);

	protected:
		glm::vec3 position_; // Position of Bullet
		glm::vec3 velocity_; // Velocity of Bullet
		int damage;
		int lifeSpan;
		int Type;
	}; // class Bullet

} // namespace game

#endif // ASTEROID_H_
