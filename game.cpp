#include <iostream>
#include <time.h>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <vector>

#include "game.h"
#include "bin/path_config.h"

namespace game {

	// Some configuration constants
	// They are written here as global variables, but ideally they should be loaded from a configuration file

	// Main window settings
	const std::string window_title_g = "Demo";
	const unsigned int window_width_g = 800;
	const unsigned int window_height_g = 600;
	const bool window_full_screen_g = false;

	// Viewport and camera settings
	float camera_near_clip_distance_g = 0.01;
	float camera_far_clip_distance_g = 1000.0;
	float camera_fov_g = 20.0; // Field-of-view of camera
	glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
	glm::vec3 camera_position_g(0.5, 0.5, 10.0);
	glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
	glm::vec3 camera_up_g(0.0, 1.0, 0.0);

	// Materials 
	const std::string material_directory_g = MATERIAL_DIRECTORY;




	Game::Game(void) {

		// Don't do work in the constructor, leave it for the Init() function
	}


	void Game::Init(void) {

		// Run all initialization steps
		InitWindow();
		InitView();
		InitEventHandlers();


		gameState = 0;

		num_bullets = 0;

		num_enemies = 0;

		missle_timer = 0;

		camera_angle_Y = 0;

		AnimationTimer = 100;

		introPhase = 0;

		enemy_spawn_timer = 300;

		affection = 0;

		currentDialogue = 0;

		// Set variables
		animating_ = true;
		paused = false;

		glfwGetCursorPos(window_, &CursorXPos, &CursorYPos);
		glfwGetCursorPos(window_, &OldCursorXPos, &OldCursorYPos);
		a_input = false;
		s_input = false;
		d_input = false;
		w_input = false;
		q_input = false;
		e_input = false;
		z_input = false;
		x_input = false;
	}


	void Game::InitWindow(void) {

		// Initialize the window management library (GLFW)
		if (!glfwInit()) {
			throw(GameException(std::string("Could not initialize the GLFW library")));
		}

		// Create a window and its OpenGL context
		if (window_full_screen_g) {
			window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
		}
		else {
			window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
		}
		if (!window_) {
			glfwTerminate();
			throw(GameException(std::string("Could not create window")));
		}

		// Make the window's context the current one
		glfwMakeContextCurrent(window_);

		// Initialize the GLEW library to access OpenGL extensions
		// Need to do it after initializing an OpenGL context
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			throw(GameException(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
		}
	}


	void Game::InitView(void) {

		// Set up z-buffer
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Set viewport
		int width, height;
		glfwGetFramebufferSize(window_, &width, &height);
		glViewport(0, 0, width, height);

		// Set up camera
		// Set current view
		camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);
		// Set projection
		camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);

		zoom = 30.0;
		firstPerson = false;
	}


	void Game::InitEventHandlers(void) {

		// Set event callbacks
		glfwSetMouseButtonCallback(window_, mouse_button_callback);
		glfwSetScrollCallback(window_, scroll_callback);
		glfwSetKeyCallback(window_, KeyCallback);
		glfwSetFramebufferSizeCallback(window_, ResizeCallback);

		// Set pointer to game object, so that callbacks can access it
		glfwSetWindowUserPointer(window_, (void *) this);
	}


	void Game::SetupResources(void) {

		// Create a sphere
		resman_.CreateSphere("SphereMesh");
		resman_.CreateTorus("TorusMesh");
		resman_.CreateCube("CubeMesh");
		resman_.CreateGround("GroundMesh");
		resman_.CreateParts("PartsMesh");
		resman_.CreateSphereParticles("SphereParticles");

		// Load material to be applied to sphere
		std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/shiny_texture");
		resman_.LoadResource(Material, "ShinyTextureMaterial", filename.c_str());



		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/fire.jpg");
		resman_.LoadResource(Texture, "Flame", filename.c_str());

		// Load texture to be applied to sphere
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/LOghan.jpg");
		resman_.LoadResource(Texture, "LOghan", filename.c_str());

		// Load texture to be applied to sphere
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/BOrdy.jpg");
		resman_.LoadResource(Texture, "BOrdy", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/grass.jpg");
		resman_.LoadResource(Texture, "Grass", filename.c_str());

		//filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/Helis.jpg");
		//resman_.LoadResource(Texture, "Helis", filename.c_str());

		// Load texture to be applied to sphere
		//filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/Tanks.jpg");
		//resman_.LoadResource(Texture, "Tanks", filename.c_str());

		//filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/Guns.jpg");
		//resman_.LoadResource(Texture, "Guns", filename.c_str());

		//filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/Waifu.jpg");
		//resman_.LoadResource(Texture, "Waifu", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/camo.jpg");
		resman_.LoadResource(Texture, "camo", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/dkmetal.jpg");
		resman_.LoadResource(Texture, "dkmetal", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/metal.jpg");
		resman_.LoadResource(Texture, "metal", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/catCamo.png");
		resman_.LoadResource(Texture, "catCamo", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/wall.png");
		resman_.LoadResource(Texture, "wall", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Textures/pcamo.png");
		resman_.LoadResource(Texture, "pcamo", filename.c_str());



		//Dialogue Textures
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/helicopterfuel.jpg");
		resman_.LoadResource(Texture, "helicopterfuel", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/meltheart.jpg");
		resman_.LoadResource(Texture, "meltheart", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/startscreen.jpg");
		resman_.LoadResource(Texture, "startscreen", filename.c_str());


		//conversation
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia1.png");
		resman_.LoadResource(Texture, "dia1", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia2.png");
		resman_.LoadResource(Texture, "dia2", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia3.png");
		resman_.LoadResource(Texture, "dia3", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia4.png");
		resman_.LoadResource(Texture, "dia4", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia5.png");
		resman_.LoadResource(Texture, "dia5", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia6.png");
		resman_.LoadResource(Texture, "dia6", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia7.png");
		resman_.LoadResource(Texture, "dia7", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia8.png");
		resman_.LoadResource(Texture, "dia8", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia9.png");
		resman_.LoadResource(Texture, "dia9", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia10.png");
		resman_.LoadResource(Texture, "dia10", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Dialogue/dia11.png");
		resman_.LoadResource(Texture, "dia11", filename.c_str());



		dialogues.push_back(resman_.GetResource("dia1"));
		dialogues.push_back(resman_.GetResource("dia2"));
		dialogues.push_back(resman_.GetResource("dia3"));
		dialogues.push_back(resman_.GetResource("dia4"));
		dialogues.push_back(resman_.GetResource("dia5"));
		dialogues.push_back(resman_.GetResource("dia6"));
		dialogues.push_back(resman_.GetResource("dia7"));
		dialogues.push_back(resman_.GetResource("dia8"));
		dialogues.push_back(resman_.GetResource("dia9"));
		dialogues.push_back(resman_.GetResource("dia10"));
		dialogues.push_back(resman_.GetResource("dia11"));


	}


	void Game::SetupScene(void) {

		// Set background color for the scene
		scene_.SetBackgroundColor(viewport_background_color_g);

		world = CreateInstance("world", "GroundMesh", "ShinyTextureMaterial", "Grass");
		world->Scale(glm::vec3(0.0, 0.0, 0.0));


		// Create an instance of the sphere mesh

		player = CreatePlayer();
		baehawk = CreateBae();

		ground = CreateInstance("Ground", "GroundMesh", "ShinyTextureMaterial", "Grass");
		ground->Scale(glm::vec3(1000, 0.0, 1000.0));
		ground->SetPosition(glm::vec3(0.0, -15.0, 0.0));


		world->AddChild(player);
		world->AddChild(baehawk);
		world->AddChild(ground);

		scene_.SetRoot(world);



	}


	void Game::MainLoop(void) {

		spawnBuildings();

		CreateHUD();


		// Loop while the user did not close the window
		while (!glfwWindowShouldClose(window_)) {


			if (!paused) {
				glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


				if (animating_ && gameState != 2) {
					static double last_time = 0;
					double current_time = glfwGetTime();
					if ((current_time - last_time) > 0.01) {
						//scene_.Update();

						// Animate the sphere
						glm::quat rotation = glm::angleAxis(glm::pi<float>() / 10.0f, glm::vec3(0.0, 0.0, 1.0));
						t_blade->Rotate(rotation);
						rotation = glm::angleAxis(glm::pi<float>() / 10.0f, glm::vec3(1.0, 0.0, 0.0));
						b_blade->Rotate(rotation);


						last_time = current_time;
					}
				}


				if (gameState == 0) { 
					Intro();
				}

				

				else if (gameState == 1) MainGame();


				else if(gameState == 2) GameOver();





				if(gameState != 0 )scene_.Update();

				scene_.Draw(&camera_);

				glfwSwapBuffers(window_);

			}
			else
				glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			// Update other events like input handling
			glfwPollEvents();
		}
	}


	void Game::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{

		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;


		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			game->CreateBullet(game->player->GetPosition(), game->camera_.GetForward(), 0);
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && game->missle_timer <= 0) {
			game->CreateMissles(game->player->GetPosition(), game->camera_.GetForward());
			game->missle_timer = 120;
		}

	}



	void Game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;

		game->zoom -= yoffset;

	}

	void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		// Get user data with a pointer to the game class
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;


		if (game->gameState == 0 && action == GLFW_RELEASE) {
			//Advance intro text
		};


		// Quit game if 'q' is pressed
		if (key == GLFW_KEY_P && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		// Stop animation if space bar is pressed
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			game->animating_ = (game->animating_ == true) ? false : true;

			game->paused = (game->paused == true) ? false : true;

		}
		//SceneNode *node = game->scene_.GetNode("SphereInstance1");

		if (game->introPhase < 3 && action == GLFW_RELEASE && key != GLFW_KEY_ESCAPE) { 
			game->introPhase = 3; 
		}
		else if (game->introPhase == 3 && action == GLFW_RELEASE) {
			if (game->currentDialogue < game->dialogues.size()-1) game->currentDialogue++;
			else {
				game->introPhase = 4;
				std::stringstream ss;

				for (int i = 1; i <= game->dialogues.size(); i++) {
					ss << i;
					game->resman_.RemoveResource("dia" + ss.str());
				
				}
					game->dialogues.erase(game->dialogues.begin(), game->dialogues.end());

					game->player->SetOrientation(game->enemies[1]->GetOrientation());
					game->baehawk->SetOrientation(game->enemies[1]->GetOrientation());



			}
			
		}


		if (!game->paused) {
			// View control
			float rot_factor(glm::pi<float>() / 180);
			float trans_factor = 1.0;


			if (glfwGetKey(window, GLFW_KEY_W)) { game->w_input = true; }
			else { game->w_input = false; }

			if (glfwGetKey(window, GLFW_KEY_S)) { game->s_input = true; }
			else { game->s_input = false; }

			if (glfwGetKey(window, GLFW_KEY_A)) { game->a_input = true; }
			else if (action == GLFW_RELEASE) { game->a_input = false; }

			if (glfwGetKey(window, GLFW_KEY_D)) { game->d_input = true; }
			else { game->d_input = false; }

			if (glfwGetKey(window, GLFW_KEY_Q)) { game->q_input = true; }
			else if (action == GLFW_RELEASE) { game->q_input = false; }

			if (glfwGetKey(window, GLFW_KEY_E)) { game->e_input = true; }
			else if (action == GLFW_RELEASE) { game->e_input = false; }

			if (glfwGetKey(window, GLFW_KEY_SPACE)) { game->z_input = true; }
			else if (action == GLFW_RELEASE) { game->z_input = false; }

			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) { game->x_input = true; }
			else if (action == GLFW_RELEASE) { game->x_input = false; }

			if (key == GLFW_KEY_1) {
				game->firstPerson = true;
				game->player->setDraw(false);
				game->camera_.SetOrientation(game->player->GetOrientation());
				game->camera_.Pitch(-4.7);
				game->camera_angle_Y = 0;
			}
			if (key == GLFW_KEY_2) {
				game->firstPerson = false;
				game->player->setDraw(true);
			}



			if (key == GLFW_KEY_T) {
				game->CreateTank(game->player->GetPosition() + glm::vec3(10, 0, 0));
			}
			if (key == GLFW_KEY_Y) {
				game->CreateHeli(game->player->GetPosition() + glm::vec3(10, 0, 0));
			}
			if (key == GLFW_KEY_U) {
				game->CreateGun(game->player->GetPosition() + glm::vec3(10, 0, 0));
			}


			if (key == GLFW_KEY_B) {
				game->player->SetTexture(game->resman_.GetResource("LOghan"));
			}

			if (key == GLFW_KEY_N) {
				game->player->SetTexture(game->resman_.GetResource("BOrdy"));
			}
		}

	}




	void Game::PlayerMovement() {

		float rot_factor(glm::pi<float>() / 180);


		camera_.SetPosition(player->GetPosition() + glm::vec3(0, 1, 0));

		glfwGetCursorPos(window_, &CursorXPos, &CursorYPos);

		if (CursorXPos > OldCursorXPos) {

			camera_.Yaw(-rot_factor*(CursorXPos - OldCursorXPos)*0.25);

			if (firstPerson) {
				float fuu = -rot_factor*(CursorXPos - OldCursorXPos)*0.25;
				player->SetOrientation(glm::angleAxis(fuu, glm::vec3(0.0, 1.0, 0.0)) * player->GetOrientation());
			}


		}
		if (CursorXPos < OldCursorXPos) {

			camera_.Yaw(rot_factor*(OldCursorXPos - CursorXPos)*0.25);

			if (firstPerson) {
				float fuu = rot_factor*(OldCursorXPos - CursorXPos)*0.25;
				player->SetOrientation(glm::angleAxis(fuu, glm::vec3(0.0, 1.0, 0.0)) * player->GetOrientation());
			}



		}
		if (CursorYPos > OldCursorYPos && camera_angle_Y < 250) {
			camera_angle_Y += CursorYPos - OldCursorYPos;
			camera_.Pitch(-rot_factor*(CursorYPos - OldCursorYPos)*0.25);


		}
		if (CursorYPos < OldCursorYPos && camera_angle_Y > -250) {
			camera_angle_Y -= OldCursorYPos - CursorYPos;
			camera_.Pitch(rot_factor*(OldCursorYPos - CursorYPos)*0.25);


		}

		camera_.Translate(-camera_.GetForward()*zoom);
		camera_.Translate(glm::vec3(0, 0.5, 0));

		if (firstPerson) camera_.SetPosition(player->GetPosition());

		glfwGetCursorPos(window_, &OldCursorXPos, &OldCursorYPos);

		if (q_input) {
			camera_.Yaw(rot_factor* 0.573f);
			player->SetOrientation(glm::angleAxis(0.01f, glm::vec3(0.0, 1.0, 0.0)) * player->GetOrientation());
		}
		if (s_input) {
			camera_.Translate(-player->GetForward()*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, player->GetSide()) * camera_.GetOrientation());
			player->Translate(-player->GetForward()*player->speed);
		}
		if (e_input) {
			//camera_.Translate(-camera_.GetSide()*player->speed);
			camera_.Yaw(-rot_factor* 0.573f);
			player->SetOrientation(glm::angleAxis(-0.01f, glm::vec3(0.0, 1.0, 0.0)) * player->GetOrientation());
			//player->Translate(-camera_.GetSide()*player->speed);
		}
		if (w_input) {
			camera_.Translate(player->GetForward()*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, player->GetSide()) * camera_.GetOrientation());
			player->Translate(player->GetForward()*player->speed);
		}

		if (a_input) {
			camera_.Translate(-player->GetSide()*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(-player->GetSide()*player->speed);
		}

		if (d_input) {
			camera_.Translate(player->GetSide()*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(player->GetSide()*player->speed);
		}

		if (z_input) {
			camera_.Translate(glm::vec3(0.0, 1.0, 0.0)*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(glm::vec3(0.0, 1.0, 0.0)*player->speed);
		}

		if (x_input) {
			camera_.Translate(-glm::vec3(0.0, 1.0, 0.0)*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(-glm::vec3(0.0, 1.0, 0.0)*player->speed);
		}

	}


	void Game::ResizeCallback(GLFWwindow* window, int width, int height) {

		// Set up viewport and camera projection based on new window size
		glViewport(0, 0, width, height);
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;
		game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
	}


	Game::~Game() {

		glfwTerminate();
	}



	//GameStates

	void Game::MainGame() {

		


		PlayerMovement();


		missle_timer--;
		
		for (int j = 0; j < enemies.size(); j++) {
			bool EnemyDed = false;

			enemies[j]->AttackRange();

			
			if (enemies[j]->isShooting() && enemies[j] != NULL) {
					CreateBullet(enemies[j]->GetPosition(), enemies[j]->getTarget()->GetPosition() - enemies[j]->GetPosition(), enemies[j]->getDamage());
			}
			for (int i = 0; i < bullets.size(); i++) {

				if (EnemyDed) continue;
				if (collision(bullets[i], enemies[j])) {

					enemies[j]->takeDamage(bullets[i]->GetDamage());
					bullets[i]->die();
					world->removeChild(bullets[i]);
					bullets.erase(bullets.begin() + (i));

					if (enemies[j]->getHealth() <= 0) {
						enemies[j]->die();
						world->removeChild(enemies[j]);
						affection++;
						enemies.erase(enemies.begin() + (j));
						EnemyDed = true;
					}

				}

			}

			



		}


	for (int i = 0; i < bullets.size(); i++) {
		if (bullets[i]->GetLifeSpan() < 0) {
			bullets[i]->die();
			world->removeChild(bullets[i]);
			bullets.erase(bullets.begin() + (i));
		}
	}

	for (int i = 0; i < enemyBullets.size(); i++) {

		if (collision(player, enemyBullets[i])) {
			enemyBullets[i]->GetDamage();
			player->takeDamage(enemyBullets[i]->GetDamage());
		}

		if (enemyBullets[i]->GetLifeSpan() < 0) {
			enemyBullets[i]->die();
			world->removeChild(enemyBullets[i]);
			enemyBullets.erase(enemyBullets.begin() + (i));
		}
	}


	spawnEnemies();

	if (player->getHealth() < 0) gameState = 2;

	Health->SetPosition(camera_.GetPosition() + camera_.GetForward()*glm::vec3(0.5, 0.5, 0.5) +camera_.GetSide() * glm::vec3(0.0, 0.0, 0.0) + glm::normalize(glm::cross(camera_.GetForward(), camera_.GetSide())) * -glm::vec3(0.08, 0.08, 0.08));
	Health->SetOrientation(camera_.GetOrientation());
	Health->SetScale(glm::vec3(player->getHealth() / 2000, 0.008, 0.0));

	//Health->Translate(glm::vec3(-0.0, -0.0, -0.5));

	Reticle->SetPosition(camera_.GetPosition() + camera_.GetForward()*glm::vec3(0.5, 0.5, 0.5));
	Reticle->SetOrientation(camera_.GetOrientation());

	baehawk->getAffection()->SetPosition(baehawk->GetPosition() - glm::vec3(0, -2, 0));
	baehawk->getAffection()->SetOrientation(camera_.GetOrientation());
	baehawk->getAffection()->SetScale(glm::vec3(affection, 0.4, 0));


}


void Game::GameOver() {
	//scene_.RemoveNode(player);
	DialogueBox->SetTexture(resman_.GetResource("Flame"));
	DialogueBox->SetScale(glm::vec3(3.0f,3.0f,3.0f));
	DialogueBox->SetPosition(player->GetPosition());
	camera_.SetPosition(player->GetPosition() + glm::vec3(0, 1, 0));
	camera_.Translate(-camera_.GetForward()*zoom);
	camera_.Translate(glm::vec3(0, 0.5, 0));
	player->Translate(glm::vec3(0,-0.2,0));


}

void Game::Intro() {
	camera_.SetPosition(glm::vec3(100000.0, 0.0, 0.0));

	DialogueBox->SetPosition((camera_.GetPosition() + camera_.GetForward()*glm::vec3(AnimationTimer, AnimationTimer, AnimationTimer))+ glm::vec3(0.0,0.0,0.0));
	
	DialogueBox->SetOrientation(camera_.GetOrientation());
	if (introPhase < 2) {
		AnimationTimer--;
		if (AnimationTimer == 3) _sleep(5000);
		if (AnimationTimer < -4 && introPhase == 0) {
			DialogueBox->SetTexture(resman_.GetResource("meltheart"));
			AnimationTimer = 100;
			introPhase = 1;
		}
		else if (AnimationTimer < -4 && introPhase == 1) { 
			introPhase = 2; 
			AnimationTimer = 3;
			DialogueBox->SetTexture(resman_.GetResource("startscreen"));
		}
	}
	else if (introPhase == 2);
	else if (introPhase == 3) {

		scene_.SetBackgroundColor(glm::vec3(0.4,0.6,1));
		camera_.SetPosition(glm::vec3(0.0, 0.0, 0.0));

		if (currentDialogue >= 3 && currentDialogue <= 4) { 
			camera_.SetPosition(glm::vec3(0.7, -12.0, 0.0)); 
			if (enemies.size()<32) {
				CreateGun(glm::vec3(-5.0, -13.0, -20.0));
				CreateHeli(glm::vec3(0.0, -13.0, -20.0));
				CreateTank(glm::vec3(5.0, -13.0, -20.0));
				glm::quat rotation = glm::angleAxis(glm::pi<float>() /1.2f, glm::vec3(0.0, 0.0, 1.0));
				enemies[30]->Rotate(rotation);
				enemies[31]->Rotate(rotation);
				enemies[32]->Rotate(rotation);

			}
		}

		player->SetPosition(glm::vec3(-1.5, -0.7, -10.0));
		player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * (glm::angleAxis(2.8f, camera_.GetForward())) * camera_.GetOrientation());
		baehawk->SetPosition(glm::vec3(1.1, -0.7, -10.0));
		baehawk->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * (glm::angleAxis(3.7f, camera_.GetForward())) * camera_.GetOrientation());
		DialogueBox->SetPosition(camera_.GetPosition() + camera_.GetForward()*glm::vec3(0.5, 0.5, 0.5) + camera_.GetSide() * glm::vec3(0.0, 0.0, 0.0) + glm::normalize(glm::cross(camera_.GetForward(), camera_.GetSide())) * -glm::vec3(-0.065, -0.065, -0.065));
		DialogueBox->SetOrientation(camera_.GetOrientation());
		DialogueBox->SetScale(glm::vec3(0.1, 0.040, 0.0));
		DialogueBox->SetTexture(dialogues[currentDialogue]);

		

	}
	else if (introPhase == 4) gameState = 1;
}




Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name){

    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat);
    //scene_.AddNode(ast);
    return ast;
}


void Game::CreateAsteroidField(int num_asteroids){

    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "SimpleSphereMesh", "ObjectMaterial");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
    }
}


SceneNode *Game::CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name){

    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

	Resource *tex = resman_.GetResource(texture_name);

    SceneNode *scn = new SceneNode(entity_name, geom, mat, tex);
    return scn;
}


Player *Game::CreatePlayer() {

	Resource *geom = resman_.GetResource("CubeMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("LOghan");


	Player *player = new Player("player", geom, mat, tex);
	//scene_.AddNode(player);

	player->SetForward(glm::angleAxis(4.7f, player->GetSide()) *camera_.GetForward());

	MakePlayer(player);

	return player;

}


BaeHawk *Game::CreateBae() {

	Resource *geom = resman_.GetResource("CubeMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("BOrdy");

	SceneNode *meter = CreateInstance("AffectionMeter", "PartsMesh", "ShinyTextureMaterial", "metal");
	meter->SetScale(glm::vec3(1,0.4,0));

	world->AddChild(meter);

	BaeHawk *bae = new BaeHawk("Bae", geom, mat, tex, meter);
	//scene_.AddNode(player);

	bae->SetForward(glm::vec3(1.0, 0.0, 0.0));
	bae->SetPosition(glm::vec3(5,0,0));

	bae->setPlayer(player);


	MakeBae(bae);

	return bae;

}

void Game::MakeBae(SceneNode* player) {
	player->Scale(glm::vec3(0.8, 1.0, 0.8));
	glm::quat rotation = glm::angleAxis(-glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0));
	player->Rotate(rotation);

	SceneNode* bbody = CreateInstance("bbody", "PartsMesh", "ShinyTextureMaterial", "pcamo");
	bbody->Scale(glm::vec3(1.0, 1.5, 1.0));
	bbody->Translate(glm::vec3(0.0, -0.3, 0.0));

	SceneNode* ftail = CreateInstance("ftail", "PartsMesh", "ShinyTextureMaterial", "pcamo");
	ftail->Scale(glm::vec3(0.7, 1.5, 0.7));
	ftail->Translate(glm::vec3(0.0, -1.5, 0.15));

	SceneNode* btail = CreateInstance("btail", "PartsMesh", "ShinyTextureMaterial", "pcamo");
	btail->Scale(glm::vec3(0.4, 1.25, 0.5));
	btail->Translate(glm::vec3(0.0, -1.25, 0.1));

	SceneNode* rtail = CreateInstance("rtail", "PartsMesh", "ShinyTextureMaterial", "pcamo");
	rtail->Scale(glm::vec3(0.3, 1.0, 0.3));
	rtail->Translate(glm::vec3(0.0, -1.0, 0.1));

	SceneNode* bwings = CreateInstance("bwings", "PartsMesh", "ShinyTextureMaterial", "pcamo");
	bwings->Scale(glm::vec3(2.0, 0.5, 0.1));
	bwings->Translate(glm::vec3(0.0, -0.5, 0.15));

	SceneNode* btailwing = CreateInstance("btailwing", "PartsMesh", "ShinyTextureMaterial", "pcamo");
	btailwing->Scale(glm::vec3(0.1, 0.35, 0.8));
	btailwing->Translate(glm::vec3(0.0, -0.05, 0.4));

	SceneNode* fwings = CreateInstance("fwings", "PartsMesh", "ShinyTextureMaterial", "pcamo");
	fwings->Scale(glm::vec3(5.0, 1.0, 0.1));
	fwings->Translate(glm::vec3(0.0, -0.15, 0.4));

	SceneNode* lmissle = CreateInstance("lmissle", "PartsMesh", "ShinyTextureMaterial", "metal");
	lmissle->Scale(glm::vec3(0.4, 1.6, 0.4));
	lmissle->Translate(glm::vec3(-2.0, -0.15, -0.25));

	SceneNode* rmissle = CreateInstance("rmissle", "PartsMesh", "ShinyTextureMaterial", "metal");
	rmissle->Scale(glm::vec3(0.4, 1.6, 0.4));
	rmissle->Translate(glm::vec3(2.0, -0.15, -0.25));

	player->AddChild(bbody);
	bbody->AddChild(ftail);
	ftail->AddChild(btail);
	btail->AddChild(rtail);
	rtail->AddChild(bwings);
	bwings->AddChild(btailwing);
	bbody->AddChild(fwings);
	fwings->AddChild(lmissle);
	fwings->AddChild(rmissle);

}


void Game::MakePlayer(Player* player) {
	player->Scale(glm::vec3(0.6, 1.5, 0.6));
	glm::quat rotation = glm::angleAxis(-glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0));
	player->Rotate(rotation);

	SceneNode* b_nose = CreateInstance("b_nose", "PartsMesh", "ShinyTextureMaterial", "camo");
	b_nose->Scale(glm::vec3(0.8, 0.2, 0.8));
	b_nose->Translate(glm::vec3(0.0, 0.5, 0.0));

	SceneNode* hell_body = CreateInstance("hell_body", "PartsMesh", "ShinyTextureMaterial", "camo");
	hell_body->Scale(glm::vec3(1.0, 1.5, 1.0));
	hell_body->Translate(glm::vec3(0.0, -0.85, 0.0));

	SceneNode* wingss = CreateInstance("wingss", "PartsMesh", "ShinyTextureMaterial", "camo");
	wingss->Scale(glm::vec3(2.0, 0.6, 0.06));
	wingss->Translate(glm::vec3(0.0, 0.15, -0.1));

	SceneNode* tails = CreateInstance("tails", "PartsMesh", "ShinyTextureMaterial", "dkmetal");
	tails->Scale(glm::vec3(0.2, 1.7, 0.2));
	tails->Translate(glm::vec3(0.0, -1.5, 0.13));

	SceneNode* b_wing = CreateInstance("b_wing", "PartsMesh", "ShinyTextureMaterial", "camo");
	b_wing->Scale(glm::vec3(0.1, 0.3, 0.6));
	b_wing->Translate(glm::vec3(-0.1, -0.7, 0.15));

	t_blade = CreateInstance("t_blades", "PartsMesh", "ShinyTextureMaterial", "dkmetal");
	t_blade->Scale(glm::vec3(3.5, 0.3, 0.05));
	t_blade->Translate(glm::vec3(0.0, 0.0, 0.6));

	SceneNode* bump = CreateInstance("bump", "PartsMesh", "ShinyTextureMaterial", "camo");
	bump->Scale(glm::vec3(0.33, 0.33, 0.2));
	bump->Translate(glm::vec3(0.0, 0.0, 0.5));

	b_blade = CreateInstance("b_blades", "PartsMesh", "ShinyTextureMaterial", "dkmetal");
	b_blade->Scale(glm::vec3(0.05, 1.25, 0.15));
	b_blade->Translate(glm::vec3(-0.05, 0.0, 0.0));

	SceneNode* hellcase = CreateInstance("hellcase", "PartsMesh", "ShinyTextureMaterial", "camo");
	hellcase->Scale(glm::vec3(0.62, 0.2, 0.62));
	hellcase->Translate(glm::vec3(0.0, 0.63, 0.0));

	player->AddChild(b_nose);
	player->AddChild(hellcase);
	b_nose->AddChild(hell_body);
	hell_body->AddChild(wingss);
	hell_body->AddChild(tails);
	tails->AddChild(b_wing);
	hell_body->AddChild(t_blade);
	hell_body->AddChild(bump);
	b_wing->AddChild(b_blade);

}


void Game::CreateBullet(glm::vec3 position, glm::vec3 velocity, int type) {
	// Create instance name
	std::stringstream ss;
	ss << num_bullets;
	std::string index = ss.str();
	std::string name = "BulletInstance" + index;


	velocity = glm::normalize(velocity);

	// Get resources
	Resource *geom = resman_.GetResource("SphereMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("metal");

	// Create asteroid instance
	Bullet *bul = new Bullet(name, geom, mat, tex, type);

	bul->Scale(glm::vec3(0.1, 0.1, 0.1));

	world->AddChild(bul);
	//scene_.AddNode(bul);


	// Set attributes of bullet

	float speed = 2;

	bul->SetPosition(position + velocity * speed);
	//bul->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));
	bul->SetVelocity(velocity * speed);


	if (type == 0) {
		speed = 3;
		bullets.push_back(bul);
	}

	else {
		bul->SetDamage(type);
		enemyBullets.push_back(bul);

	}

}


void Game::CreateMissles(glm::vec3 position, glm::vec3 Forwardvelocity) {
	for (float j = -1; j < 2; j++) {

		for (float i = -2; i < 3; i++) {
			if (i == 0) i++;
			// Create instance name
			std::stringstream ss;
			ss << num_missles;
			std::string index = ss.str();
			std::string name = "MissleInstance" + index;


			// Get resources
			Resource *geom = resman_.GetResource("SphereMesh");
			if (!geom) {
				throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
			}

			Resource *mat = resman_.GetResource("ShinyTextureMaterial");
			if (!mat) {
				throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
			}

			Resource *tex = resman_.GetResource("metal");

			// Create asteroid instance
			Missle *bul = new Missle(name, geom, mat, tex, Forwardvelocity);

			bul->Scale(glm::vec3(0.3, 0.3, 0.3));

			//scene_.AddNode(bul);
			world->AddChild(bul);

			// Set attributes of bullet

			float speed = 0.025;

			glm::vec3 velocity = camera_.GetSide() * i + glm::vec3(0, j, 0);

			//velocity += glm::vec3(i,0,0);

			bul->SetPosition(position);
			//bul->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));
			bul->SetVelocity(velocity * speed);

			bullets.push_back(bul);

		}
	}
}


void Game::CreateTank(glm::vec3 position) {
	// Create instance name
	std::stringstream ss;
	ss << num_enemies;
	std::string index = ss.str();
	std::string name = "EnemyInstance" + index;


	// Get resources
	Resource *geom = resman_.GetResource("PartsMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("catCamo");

	// Create asteroid instance
	Tanks *enemy = new Tanks(name, geom, mat, tex);
	enemy->setTarget(player);
	enemy->SetForward(glm::vec3(1.0, 0.0, 0.0));

	MakeTank(enemy);

	//scene_.AddNode(enemy);
	world->AddChild(enemy);


	// Set attributes of bullet

	float speed = 5;

	enemy->SetPosition(position);
	enemy->setPatrolPoint(position);
	//bul->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));

	enemies.push_back(enemy);

}

void Game::CreateGun(glm::vec3 position) {
	// Create instance name
	std::stringstream ss;
	ss << num_enemies;
	std::string index = ss.str();
	std::string name = "EnemyInstance" + index;


	// Get resources
	Resource *geom = resman_.GetResource("PartsMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("catCamo");

	// Create asteroid instance
	Guns *enemy = new Guns(name, geom, mat, tex);
	enemy->setTarget(player);
	enemy->SetForward(glm::vec3(1.0, 0.0, 0.0));

	MakeGunner(enemy);

	//scene_.AddNode(enemy);
	world->AddChild(enemy);

	// Set attributes of bullet

	float speed = 5;

	enemy->setPatrolPoint(position);
	enemy->SetPosition(position);
	//bul->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));

	enemies.push_back(enemy);

}

void Game::CreateHeli(glm::vec3 position) {
	// Create instance name
	std::stringstream ss;
	ss << num_enemies;
	std::string index = ss.str();
	std::string name = "EnemyInstance" + index;


	// Get resources
	Resource *geom = resman_.GetResource("PartsMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("catCamo");

	// Create asteroid instance
	Helis *enemy = new Helis(name, geom, mat, tex);
	enemy->setTarget(player);
	enemy->SetForward(glm::vec3(1.0, 0.0, 0.0));
	MakeHelli(enemy);

	//scene_.AddNode(enemy);
	world->AddChild(enemy);

	// Set attributes of bullet

	float speed = 5;

	enemy->setPatrolPoint(position);
	enemy->SetPosition(position);
	//bul->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));

	enemies.push_back(enemy);

}

void Game::MakeTank(SceneNode* enemy) {
	enemy->Scale(glm::vec3(1.5, 3.0, 0.5));
	enemy->SetPosition(glm::vec3(0.0, 0.0, 0.0));
	glm::quat rotation = glm::angleAxis(-glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0));
	enemy->Rotate(rotation);

	SceneNode *wheel1 = CreateInstance("wheel1", "PartsMesh", "ShinyTextureMaterial", "dkmetal");
	wheel1->Scale(glm::vec3(0.5, 2.8, 0.6));
	wheel1->SetPosition(glm::vec3(0.99, 0.0, -0.25));

	SceneNode *wheel2 = CreateInstance("wheel2", "PartsMesh", "ShinyTextureMaterial", "dkmetal");
	wheel2->Scale(glm::vec3(0.5, 2.8, 0.6));
	wheel2->SetPosition(glm::vec3(-0.99, 0.0, -0.25));

	SceneNode *gun_roof = CreateInstance("gun_roof", "PartsMesh", "ShinyTextureMaterial", "catCamo");
	gun_roof->Scale(glm::vec3(1.5, 2.15, 0.75));
	gun_roof->SetPosition(glm::vec3(0.0, -0.42, 0.61));

	SceneNode *track_cover = CreateInstance("track_cover", "PartsMesh", "ShinyTextureMaterial", "catCamo");
	track_cover->Scale(glm::vec3(2.5, 2.9, 0.1));
	track_cover->SetPosition(glm::vec3(0.0, 0.0, 0.22));

	SceneNode *gun = CreateInstance("gun", "PartsMesh", "ShinyTextureMaterial", "metal");
	rotation = glm::angleAxis(-glm::pi<float>() / -5.0f, glm::vec3(1.0, 0.0, 0.0));
	gun->Rotate(rotation);
	gun->Scale(glm::vec3(0.25, 2.25, 0.25));
	gun->SetPosition(glm::vec3(0.0, 0.75, 0.5));

	SceneNode *body = CreateInstance("body", "PartsMesh", "ShinyTextureMaterial", "catCamo");
	body->Scale(glm::vec3(1.55, 3.0, 0.55));
	body->SetPosition(glm::vec3(0.0, -0.01, 0.0));

	enemy->AddChild(wheel1);
	enemy->AddChild(wheel2);
	enemy->AddChild(gun_roof);
	enemy->AddChild(track_cover);
	enemy->AddChild(body);
	enemy->AddChild(gun);
}


void Game::MakeGunner(SceneNode *enemy) {
	enemy->Scale(glm::vec3(0.5, 0.5, 0.05));
	glm::quat rotation = glm::angleAxis(-glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0));
	enemy->Rotate(rotation);



	SceneNode* wall = CreateInstance("wall", "PartsMesh", "ShinyTextureMaterial", "catCamo");
	wall->Scale(glm::vec3(0.45, 0.1, 0.375));
	wall->SetPosition(glm::vec3(0.0, 0.2, 0.2));

	SceneNode* gun = CreateInstance("gun", "PartsMesh", "ShinyTextureMaterial", "metal");
	gun->Scale(glm::vec3(0.05, 0.4, 0.05));
	rotation = glm::angleAxis(-glm::pi<float>() / -5.0f, glm::vec3(1.0, 0.0, 0.0));
	gun->Rotate(rotation);
	gun->Translate(glm::vec3(0.0, 0.35, 0.3));

	SceneNode* body = CreateInstance("body", "PartsMesh", "ShinyTextureMaterial", "catCamo");
	body->Scale(glm::vec3(0.52, 0.52, 0.07));



	enemy->AddChild(body);
	enemy->AddChild(wall);
	enemy->AddChild(gun);
}

void Game::MakeHelli(SceneNode *enemy) {
	enemy->Scale(glm::vec3(1.0, 2.0, 1.0));
	glm::quat rotation = glm::angleAxis(-glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0));
	enemy->Rotate(rotation);
	enemy->SetPosition(glm::vec3(0.0, 30.0, -2.0));


	SceneNode* t_blades = CreateInstance("t_blades", "PartsMesh", "ShinyTextureMaterial", "dkmetal");
	t_blades->SetOrientation(glm::angleAxis(0.0f, camera_.GetForward()) * camera_.GetOrientation());
	t_blades->SetPosition(glm::vec3(0.0, 0.5, 0.5));
	t_blades->Scale(glm::vec3(4.0, 0.25, 0.01));


	SceneNode* tail = CreateInstance("tail", "PartsMesh", "ShinyTextureMaterial", "dkmetal");
	tail->Scale(glm::vec3(0.25, 1.5, 0.25));
	tail->SetPosition(glm::vec3(0.0, -1.5, 0.2));

	SceneNode* wings = CreateInstance("wings", "PartsMesh", "ShinyTextureMaterial", "catCamo");
	wings->Scale(glm::vec3(1.8, 0.4, 0.1));
	wings->SetPosition(glm::vec3(0.0, 0.25, -0.15));

	SceneNode* b_blades = CreateInstance("b_blades", "PartsMesh", "ShinyTextureMaterial", "dkmetal");
	b_blades->Scale(glm::vec3(0.01, 0.1, 1.0));
	b_blades->SetPosition(glm::vec3(-0.1, -0.7, 0.0));

	SceneNode *body = CreateInstance("hellBody", "PartsMesh", "ShinyTextureMaterial", "catCamo");
	body->Scale(glm::vec3(1.01, 1.99, 1.01));
	body->SetPosition(glm::vec3(0.0, -0.01, 0.0));


	enemy->AddChild(body);
	enemy->AddChild(t_blade);
	enemy->AddChild(tail);
	enemy->AddChild(wings);
	tail->AddChild(b_blade);
}


bool Game::collision(SceneNode *node1, SceneNode *node2) {

	glm::vec3 s = node1->GetPosition() - node2->GetPosition(); // vector between the centers of each sphere
	//std::vector<double> v = node1->velocity - node2->vel; // relative velocity between spheres
	float r = node1->GetRadius() + node2->GetRadius();

	double c = glm::dot(s,s) - r*r; // if negative, they overlap
	if (c < 0.0) // if true, they already overlap
	{
		return true;
	}

	return false;
}


void Game::CreateHUD() {
	//Create HUD
	Health = CreateInstance("Health", "GroundMesh", "ShinyTextureMaterial", "metal");
	Health->SetScale(glm::vec3(0.1, 0.008, 0.00));

	world->AddChild(Health);

	Reticle = CreateInstance("Reticle", "GroundMesh", "ShinyTextureMaterial", "metal");
	Reticle->SetScale(glm::vec3(0.001, 0.001, 0.00));

	world->AddChild(Reticle);

	DialogueBox = CreateInstance("DialogueBox", "PartsMesh", "ShinyTextureMaterial", "helicopterfuel");
	DialogueBox->SetScale(glm::vec3(1, 1, 0));

	world->AddChild(DialogueBox);

	
	//End of Create HUD

}


void Game::spawnEnemies(void) {

	if (enemies.size() < 51 && enemy_spawn_timer < 0) {

		int i = rand() % 2;
		if (i == 0)
			CreateTank(glm::vec3(rand() % 1000 - 500, 0, rand() % 1000 - 500));
		else if (i == 1)
			CreateHeli(glm::vec3(rand() % 1000 - 500, rand() % 20 + 10, rand() % 1000 - 500));

		enemy_spawn_timer = 150;

	}
	
	enemy_spawn_timer--;

}

void Game::spawnBuildings(void) {

	std::stringstream ss;
	std::string index;
	std::string name;

	float x, y, z;


	for (int i = 0; i < 60; i++) {
		ss << i;
		index = ss.str();
		name = "BuildingInstance" + index;


		x = rand() % 1000 - 500;
		y = rand() % 10 + 2;
		z = rand() % 1000 - 500;

		SceneNode *newBuilding = CreateInstance(name, "PartsMesh", "ShinyTextureMaterial", "wall");
		newBuilding->SetScale(glm::vec3(rand() % 6 + 2, y, rand() % 6 + 3));
		newBuilding->SetPosition(glm::vec3(x, -13, z));
		world->AddChild(newBuilding);

		if(i % 2 == 0)
		CreateGun(glm::vec3(x, y / 2 - 13, z));

		buildings.push_back(newBuilding);


	}




}

void Game::ViewOrtho(int x, int y)							// Set Up An Ortho View
{
	glMatrixMode(GL_PROJECTION);					// Select Projection
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho(0, x, y, 0, -1, 1);				// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);					// Select Modelview Matrix
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
}

void Game::ViewPerspective(void)							// Set Up A Perspective View
{
	glMatrixMode(GL_PROJECTION);					// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode(GL_MODELVIEW);					// Select Modelview
	glPopMatrix();							// Pop The Matrix
}

} // namespace game


