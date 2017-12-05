#ifndef BaeHawk_H_
#define BaeHawk_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"
#include "Player.h"

namespace game {

	// Abstraction of an asteroid
	class BaeHawk : public SceneNode {

	public:
		// Create asteroid from given resources
		BaeHawk(const std::string name, const Resource *geometry, const Resource *material, const Resource *tex, SceneNode *meter);

		// Destructor
		~BaeHawk();

	

		// Get/set attributes specific to asteroids
		glm::quat GetAngM(void) const;
		void SetAngM(glm::quat angm);


		// Update geometry configuration
		void Update(void);

		int getHealth();

		void takeDamage(int);

		void setDraw(bool);

		void setPlayer(Player *pla);

		SceneNode* getAffection();


	private:
		// Angular momentum of asteroid
		glm::quat angm_;

		float health;

		Player *player;

		SceneNode *Affection;

	}; // class Asteroid

} // namespace game

#endif // ASTEROID_H_
