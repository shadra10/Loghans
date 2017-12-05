#ifndef PLAYER_H_
#define PLAYER_H_

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

	// Abstraction of an asteroid
	class Player : public SceneNode {

	public:
		// Create asteroid from given resources
		Player(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex);

		// Destructor
		~Player();

	

		// Get/set attributes specific to asteroids
		glm::quat GetAngM(void) const;
		void SetAngM(glm::quat angm);


		// Update geometry configuration
		void Update(void);

		float getHealth();

		void takeDamage(int);

		void setDraw(bool);
		


	private:
		// Angular momentum of asteroid
		glm::quat angm_;

		float health;

	}; // class Asteroid

} // namespace game

#endif // ASTEROID_H_
