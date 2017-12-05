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
#include "Enemy.h"
#include "Guns.h"
#include "Helis.h"
#include "Tanks.h"
#include "missle.h"
#include "BaeHawk.h"

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

			void MakeTank(SceneNode *enemy);
			//SceneNode* world;
		

			void MakeGunner(SceneNode *enemy);

			void MakeHelli(SceneNode *enemy);

			void MakePlayer(Player* player);

			void MakeBae(SceneNode* player);

        private:

			SceneNode *world, *ground, *t_blade, *tail, *wings, *b_blade;

			Player *player;
			BaeHawk * baehawk;

			std::vector<Enemy *> enemies;
			std::vector<Bullet *> bullets;
			std::vector<Bullet *> enemyBullets;
			std::vector<SceneNode*> buildings;
			std::vector<Resource*> dialogues;


			int currentDialogue;

			int missle_timer, enemy_spawn_timer;

			float zoom;

			bool firstPerson;
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

			int AnimationTimer;

			int introPhase;

			int affection;

			SceneNode *Health, *Reticle, *DialogueBox;

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);
 
            // Methods to handle events
			static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
			static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ResizeCallback(GLFWwindow* window, int width, int height);

			void PlayerMovement();

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

			void spawnEnemies(void);

			void spawnBuildings(void);
			

            // Create an instance of an object stored in the resource manager
            SceneNode *CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""));

			// Create an instance of an object stored in the resource manager
			Player *CreatePlayer();
			BaeHawk *CreateBae();



			void MainGame();
			void GameOver(void);
			void Intro();


			void CreateHUD();

			void ViewOrtho(int,int);
			void ViewPerspective();

    }; // class Game

} // namespace game

#endif // GAME_H_
