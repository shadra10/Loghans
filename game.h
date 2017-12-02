#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "asteroid.h"
#include "Player.h"
#include "bullet.h"
#include "missle.h"
#include "Enemy.h"
#include "Guns.h"
#include "Helis.h"
#include "Tanks.h"

namespace game {

    // Exception type for the game
    class GameException: public std::exception
    {
        private:
            std::string message_;
        public:
            GameException(std::string message) : message_(message) {};
            virtual const char* what() const throw() { return message_.c_str(); };
            virtual ~GameException() throw() {};
    };

    // Game application
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();
            // Call Init() before calling any other method
            void Init(void); 
            // Set up resources for the game
            void SetupResources(void);
            // Set up initial scene
            void SetupScene(void);
            // Run the game: keep the application active
            void MainLoop(void); 

        private:

			SceneNode *player;

			std::vector<Enemy *> enemies;
			std::vector<Bullet *> bullets;
			std::vector<Bullet *> enemyBullets;


			int missle_timer;

			float zoom;
            // GLFW window
            GLFWwindow* window_;

            // Scene graph containing all nodes to render
            SceneGraph scene_;

            // Resources available to the game
            ResourceManager resman_;

            // Camera abstraction
            Camera camera_;

            // Flag to turn animation on/off
            bool animating_;

			bool paused;

			bool w_input, s_input, a_input, d_input, q_input, e_input, z_input, x_input;

			double CursorXPos, CursorYPos, OldCursorXPos, OldCursorYPos;

			double camera_angle_Y;

			int gameState;

			int num_enemies, num_bullets, num_missles;

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);
 
            // Methods to handle events
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ResizeCallback(GLFWwindow* window, int width, int height);


            // Asteroid field
            // Create instance of one asteroid
            Asteroid *CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name);
            // Create entire random asteroid field
            void CreateAsteroidField(int num_asteroids = 1500);

			void CreateBullet(glm::vec3 position, glm::vec3 velocity, int type);

			void CreateMissles(glm::vec3 position, glm::vec3 velocity);

			void CreateTank(glm::vec3 position);
			void CreateGun(glm::vec3 position);
			void CreateHeli(glm::vec3 position);

			bool collision(SceneNode *node1, SceneNode *node2);

            // Create an instance of an object stored in the resource manager
            SceneNode *CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""));

			// Create an instance of an object stored in the resource manager
			SceneNode *CreatePlayer();




			void GameOver(void);

    }; // class Game

} // namespace game

#endif // GAME_H_
