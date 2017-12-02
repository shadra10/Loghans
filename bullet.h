#ifndef BULLET_H_
#define BULLET_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>


#include "resource.h"
#include "scene_node.h"

namespace game {

	// Abstraction of an Bullet
	class Bullet : public SceneNode {

	public:
		// Create asteroid from given resources
		Bullet(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex);

		// Destructor
		~Bullet();

		void SetVelocity(glm::vec3 velocity);

		// Update geometry configuration
		void Update(void);

	private:
		glm::vec3 position_; // Position of Bullet
		glm::vec3 velocity_; // Velocity of Bullet
	}; // class Bullet

} // namespace game

#endif // ASTEROID_H_
