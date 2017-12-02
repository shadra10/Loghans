#ifndef Missle_H_
#define Missle_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>


#include "resource.h"
#include "scene_node.h"
#include "bullet.h"

namespace game {

	// Abstraction of an Missle
	class Missle : public Bullet {

	public:
		// Create asteroid from given resources
		Missle(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex, glm::vec3 forward);

		// Destructor
		~Missle();

		void SetVelocity(glm::vec3 velocity);

		// Update geometry configuration
		void Update(void);

		void die();

	private:
		glm::vec3 position_; // Position of Missle
		glm::vec3 velocity_; // Velocity of Missle
		glm::vec3 forwardVelocity;

	}; // class Missle

} // namespace game

#endif // ASTEROID_H_
