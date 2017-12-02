#include <iostream>
#include <time.h>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

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
const glm::vec3 viewport_background_color_g(1.0, 1.0, 1.0);
glm::vec3 camera_position_g(0.5, 0.5, 10.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;




Game::Game(void){

    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void){

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();


	gameState = 0;

	num_bullets = 0;

	num_enemies = 0;

	missle_timer = 0;

	camera_angle_Y = 0;

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

       
void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!window_){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){

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

	zoom = 15.0;
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){

    // Create a sphere
    resman_.CreateSphere("SphereMesh");
	resman_.CreateTorus("TorusMesh");
	resman_.CreateCube("CubeMesh");
	resman_.CreateGround("GroundMesh");

    // Load material to be applied to sphere
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/shiny_texture");
    resman_.LoadResource(Material, "ShinyTextureMaterial", filename.c_str());

    // Load texture to be applied to sphere
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/LOghan.jpg");
    resman_.LoadResource(Texture, "LOghan", filename.c_str());

	// Load texture to be applied to sphere
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/BOrdy.jpg");
	resman_.LoadResource(Texture, "BOrdy", filename.c_str());

	filename = std::string(MATERIAL_DIRECTORY) + std::string("/grass.jpg");
	resman_.LoadResource(Texture, "Grass", filename.c_str());

	// Load texture to be applied to sphere
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/Helis.jpg");
	resman_.LoadResource(Texture, "Helis", filename.c_str());

	// Load texture to be applied to sphere
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/Tanks.jpg");
	resman_.LoadResource(Texture, "Tanks", filename.c_str());

	filename = std::string(MATERIAL_DIRECTORY) + std::string("/Guns.jpg");
	resman_.LoadResource(Texture, "Guns", filename.c_str());

	filename = std::string(MATERIAL_DIRECTORY) + std::string("/Waifu.jpg");
	resman_.LoadResource(Texture, "Waifu", filename.c_str());


}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);

    // Create an instance of the sphere mesh

    player = CreatePlayer();
	
    player->Scale(glm::vec3(1.0, 3.0, 1.0));
    glm::quat rotation = glm::angleAxis(-glm::pi<float>()/2.0f, glm::vec3(1.0, 0.0, 0.0));
    player->Rotate(rotation);
    player->Translate(glm::vec3(0.0, 0.0, -2.0));


	game::SceneNode *ground = CreateInstance("Ground", "GroundMesh", "ShinyTextureMaterial", "Grass");
	ground->Scale(glm::vec3(1000, 1.0, 1000.0));
	ground->Translate(glm::vec3(0.0, -15.0, 0.0));

	game::SceneNode *t_blade = CreateInstance("t_blade", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	t_blade->Scale(glm::vec3(5.0, 0.25, 0.01));
	t_blade->Translate(glm::vec3(0.0, 0.5, -2.0));

	t_blade->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
	//t_blade->Translate(-camera_.GetSide()*player->speed);

	


	

	
}


void Game::MainLoop(void){


	float rot_factor(glm::pi<float>() / 180);

    // Loop while the user did not close the window
	while (!glfwWindowShouldClose(window_)) {

		if (gameState == 2){

			GameOver();

		}


        // Animate the scene
		SceneNode *node = scene_.GetNode("t_blade");
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.01){
                scene_.Update();

                // Animate the sphere
				node = scene_.GetNode("t_blade");
                glm::quat rotation = glm::angleAxis(glm::pi<float>()/10.0f, glm::vec3(0.0, 0.0, 1.0));
                node->Rotate(rotation);


                last_time = current_time;
            }
        }


		if (!paused) {
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


			camera_.SetPosition(scene_.GetNode("player")->GetPosition()+ glm::vec3(0,1,0));

			glfwGetCursorPos(window_, &CursorXPos, &CursorYPos);

			if (CursorXPos > OldCursorXPos) {

				camera_.Yaw(-rot_factor*(CursorXPos - OldCursorXPos)*0.25);


			}
			if (CursorXPos < OldCursorXPos) {

				camera_.Yaw(rot_factor*(OldCursorXPos - CursorXPos)*0.25);


			}
			if (CursorYPos > OldCursorYPos  && camera_angle_Y < 250) {

				camera_angle_Y += CursorYPos - OldCursorYPos;
				camera_.Pitch(-rot_factor*(CursorYPos - OldCursorYPos)*0.25);


			}
			if (CursorYPos < OldCursorYPos && camera_angle_Y > -250) {

				camera_angle_Y -= OldCursorYPos - CursorYPos;
				camera_.Pitch(rot_factor*(OldCursorYPos - CursorYPos)*0.25);


			}

			camera_.Translate(-camera_.GetForward()*zoom);

			glfwGetCursorPos(window_, &OldCursorXPos, &OldCursorYPos);

		}
		else
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	
		if (a_input) {
			camera_.Yaw(rot_factor* 0.573f);
			player->SetOrientation(glm::angleAxis(0.01f, glm::vec3(0.0, 1.0, 0.0)) * player->GetOrientation());
			//player->Translate(-camera_.GetSide()*player->speed);

			node->SetOrientation(glm::angleAxis(4.7f, player->GetSide()) * player->GetOrientation());
			node->Translate(-camera_.GetSide()*player->speed);

		}
		if (s_input) {
			camera_.Translate(-player->GetForward()*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, player->GetSide()) * camera_.GetOrientation());
			player->Translate(-player->GetForward()*player->speed);

			node->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			node->Translate(-camera_.GetForward()*player->speed);
		}
		if (d_input) {
			//camera_.Translate(-camera_.GetSide()*player->speed);
			camera_.Yaw(-rot_factor* 0.573f);
			player->SetOrientation(glm::angleAxis(-0.01f, glm::vec3(0.0, 1.0, 0.0)) * player->GetOrientation());
			//player->Translate(-camera_.GetSide()*player->speed);

			node->SetOrientation(glm::angleAxis(4.7f, player->GetSide()) * player->GetOrientation());
			node->Translate(-camera_.GetSide()*player->speed);
		}
		if (w_input) {
			camera_.Translate(player->GetForward()*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, player->GetSide()) * camera_.GetOrientation());
			player->Translate(player->GetForward()*player->speed);

			node->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			node->Translate(camera_.GetForward()*player->speed);
		}

		if (q_input) {
			camera_.Translate(-player->GetSide()*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(-player->GetSide()*player->speed);

			node->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			node->Translate(camera_.GetSide()*player->speed);
		}

		if (e_input) {
			camera_.Translate(player->GetSide()*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(player->GetSide()*player->speed);

			node->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			node->Translate(camera_.GetSide()*player->speed);
		}

		if (z_input) {
			camera_.Translate(glm::vec3(0.0, 1.0, 0.0)*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(glm::vec3(0.0, 1.0, 0.0)*player->speed);

			node->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			node->Translate(camera_.GetSide()*player->speed);
		}

		if (x_input) {
			camera_.Translate(-glm::vec3(0.0, 1.0, 0.0)*player->speed);
			//player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(-glm::vec3(0.0,1.0,0.0)*player->speed);

			node->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			node->Translate(camera_.GetSide()*player->speed);
		}


		missle_timer--;

		for (int j = 0; j < enemies.size(); j++) {
			bool EnemyDed = false;
			if (enemies[j]->isShooting() && enemies[j] != NULL) {
				CreateBullet(enemies[j]->GetPosition(), player->GetPosition()- enemies[j]->GetPosition(), enemies[j]->getDamage());
			}
			for (int i = 0; i < bullets.size(); i++) {

				if (EnemyDed) continue;
				if(collision(bullets[i], enemies[j])){
					enemies[j]->die();
					bullets[i]->die();
					scene_.RemoveNode(enemies[j]);
					scene_.RemoveNode(bullets[i]);
					enemies.erase(enemies.begin() + (j));
					bullets.erase(bullets.begin() + (i));
					EnemyDed = true;
				}

			}

			
		
		}


		for (int i = 0; i < bullets.size(); i++) {
			if (bullets[i]->GetLifeSpan() < 0) {
				bullets[i]->die();
				scene_.RemoveNode(bullets[i]);
				bullets.erase(bullets.begin() + (i));
			}
		}
		

		scene_.Update();


        // Draw the scene
        scene_.Draw(&camera_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;




    // Quit game if 'q' is pressed
    if (key == GLFW_KEY_P && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    // Stop animation if space bar is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		game->animating_ = (game->animating_ == true) ? false : true;

		game->paused = (game->paused == true) ? false : true;

	}
	//SceneNode *node = game->scene_.GetNode("SphereInstance1");

	if (!game->paused) {
		// View control
		float rot_factor(glm::pi<float>() / 180);
		float trans_factor = 1.0;

		if (key == GLFW_KEY_W && action == GLFW_PRESS) {game->w_input = true;}
		else if(action == GLFW_RELEASE){ game->w_input = false; }

		if (key == GLFW_KEY_S && action == GLFW_PRESS) {game->s_input = true;}
		else if (action == GLFW_RELEASE) { game->s_input = false; }

		if (key == GLFW_KEY_A && action == GLFW_PRESS) { game->a_input = true; }
		else if (action == GLFW_RELEASE) { game->a_input = false; }

		if (key == GLFW_KEY_D && action == GLFW_PRESS) {game->d_input = true;}
		else if (action == GLFW_RELEASE) { game->d_input = false; }

		if (key == GLFW_KEY_Q && action == GLFW_PRESS) { game->q_input = true; }
		else if (action == GLFW_RELEASE) { game->q_input = false; }

		if (key == GLFW_KEY_E && action == GLFW_PRESS) { game->e_input = true; }
		else if (action == GLFW_RELEASE) { game->e_input = false; }

		if (key == GLFW_KEY_Z && action == GLFW_PRESS) { game->z_input = true; }
		else if (action == GLFW_RELEASE) { game->z_input = false; }

		if (key == GLFW_KEY_X && action == GLFW_PRESS) { game->x_input = true; }
		else if (action == GLFW_RELEASE) { game->x_input = false; }


		if (key == GLFW_KEY_1) {
			game->zoom++;
		}
		if (key == GLFW_KEY_2) {
			game->zoom--;
		}

		if (key == GLFW_KEY_G) {
			game->CreateBullet(game->player->GetPosition(), game->camera_.GetForward(), 0);
			
		}


		if (key == GLFW_KEY_F && game->missle_timer <= 0) {
			game->CreateMissles(game->player->GetPosition(), game->camera_.GetForward());
			game->missle_timer = 120;

		}


		if (key == GLFW_KEY_T) {
			game->CreateTank(game->player->GetPosition()+glm::vec3(10,0,0));
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

		if (key == GLFW_KEY_C) {
			game->player->SetTexture(game->resman_.GetResource("Tanks"));
		}

		if (key == GLFW_KEY_V) {
			game->player->SetTexture(game->resman_.GetResource("Guns"));
		}
		if (key == GLFW_KEY_M) {
			game->player->SetTexture(game->resman_.GetResource("Helis"));
		}
		if (key == GLFW_KEY_COMMA) {
			game->player->SetTexture(game->resman_.GetResource("Waifu"));
		}

	}

}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();
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
    scene_.AddNode(ast);
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

    SceneNode *scn = scene_.CreateNode(entity_name, geom, mat, tex);
    return scn;
}


SceneNode *Game::CreatePlayer() {

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

	player->SetForward(glm::angleAxis(4.7f, player->GetSide()) *camera_.GetForward());

	scene_.AddNode(player);
	return player;


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

	Resource *tex = resman_.GetResource("LOghan");

	// Create asteroid instance
	Bullet *bul = new Bullet(name, geom, mat, tex, type);

	bul->Scale(glm::vec3(0.3, 0.3, 0.3));

	scene_.AddNode(bul);


	// Set attributes of bullet

	float speed = 1;

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

			Resource *tex = resman_.GetResource("BOrdy");

			// Create asteroid instance
			Missle *bul = new Missle(name, geom, mat, tex, Forwardvelocity);

			bul->Scale(glm::vec3(0.3,0.3,0.3));

			scene_.AddNode(bul);


			// Set attributes of bullet

			float speed = 0.025;

			glm::vec3 velocity = camera_.GetSide() * i + glm::vec3(0,j,0);

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
	Resource *geom = resman_.GetResource("SphereMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("Tanks");

	// Create asteroid instance
	Tanks *enemy = new Tanks(name, geom, mat, tex);


	scene_.AddNode(enemy);


	// Set attributes of bullet

	float speed = 5;

	
	enemy->SetPosition(position);
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
	Resource *geom = resman_.GetResource("SphereMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("Guns");

	// Create asteroid instance
	Guns *enemy = new Guns(name, geom, mat, tex);


	scene_.AddNode(enemy);


	// Set attributes of bullet

	float speed = 5;


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
	Resource *geom = resman_.GetResource("SphereMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("Helis");

	// Create asteroid instance
	Helis *enemy = new Helis(name, geom, mat, tex);


	scene_.AddNode(enemy);


	// Set attributes of bullet

	float speed = 5;


	enemy->SetPosition(position);
	//bul->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));

	enemies.push_back(enemy);

}



bool Game::collision(SceneNode *node1, SceneNode *node2) {

	glm::vec3 s = node1->GetPosition() - node2->GetPosition(); // vector between the centers of each sphere
	//std::vector<double> v = node1->velocity - node2->vel; // relative velocity between spheres
	float r = node1->GetRadius() + node2->GetRadius();

	double c = glm::dot(s,s) - r*r; // if negative, they overlap
	if (c < 0.0) // if true, they already overlap
	{
		std::cout << "Collide";
		return true;
	}

	return false;
}

void Game::GameOver() {
	//scene_.RemoveNode(player);


}

} // namespace game


