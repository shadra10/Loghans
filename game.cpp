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

    // Set variables
    animating_ = true;
	paused = false;

	glfwGetCursorPos(window_, &CursorXPos, &CursorYPos);
	glfwGetCursorPos(window_, &OldCursorXPos, &OldCursorYPos);
	a_input = false;
	s_input = false;
	d_input = false;
	w_input = false;
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
}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);

	world = CreateInstance("world", "GroundMesh", "ShinyTextureMaterial", "Grass");
	world->Scale(glm::vec3(0.0, 0.0, 0.0));


    // Create an instance of the sphere mesh

    player = CreatePlayer();

	ground = CreateInstance("Ground", "GroundMesh", "ShinyTextureMaterial", "Grass");
	ground->Scale(glm::vec3(1000, 1.0, 1000.0));
	ground->SetPosition(glm::vec3(0.0, -15.0, 0.0));

	
	world->AddChild(player);
	world->AddChild(ground);

	scene_.SetRoot(world);
	
}


void Game::MainLoop(void){


	float rot_factor(glm::pi<float>() / 180);

    // Loop while the user did not close the window
	while (!glfwWindowShouldClose(window_)) {

		if (gameState == 2){

			GameOver();

		}


        // Animate the scene
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.01){
                scene_.Update();

                // Animate the sphere
                glm::quat rotation = glm::angleAxis(glm::pi<float>()/10.0f, glm::vec3(0.0, 0.0, 1.0));
				t_blade->Rotate(rotation);
				rotation = glm::angleAxis(glm::pi<float>() / 10.0f, glm::vec3(1.0, 0.0, 0.0));
				b_blade->Rotate(rotation);


                last_time = current_time;
            }
        }


		if (!paused) {
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


			camera_.SetPosition(player->GetPosition()+ glm::vec3(0,1,0));

			glfwGetCursorPos(window_, &CursorXPos, &CursorYPos);

			if (CursorXPos > OldCursorXPos) {

				camera_.Yaw(-rot_factor*(CursorXPos - OldCursorXPos)*0.25);


			}
			if (CursorXPos < OldCursorXPos) {

				camera_.Yaw(rot_factor*(OldCursorXPos - CursorXPos)*0.25);


			}
			if (CursorYPos > OldCursorYPos) {

				camera_.Pitch(-rot_factor*(CursorYPos - OldCursorYPos)*0.25);


			}
			if (CursorYPos < OldCursorYPos) {

				camera_.Pitch(rot_factor*(OldCursorYPos - CursorYPos)*0.25);


			}

			camera_.Translate(-camera_.GetForward()*zoom);

			glfwGetCursorPos(window_, &OldCursorXPos, &OldCursorYPos);

		}
		else
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	
		if (a_input) {
			camera_.Translate(-camera_.GetSide()*player->speed);
			player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(-camera_.GetSide()*player->speed);

		}
		if (s_input) {
			camera_.Translate(-camera_.GetForward()*player->speed);
			player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(-camera_.GetForward()*player->speed);

		}
		if (d_input) {
			camera_.Translate(camera_.GetSide()*player->speed);
			player->SetOrientation(glm::angleAxis(4.7f,camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(camera_.GetSide()*player->speed);

		}
		if (w_input) {
			camera_.Translate(camera_.GetForward()*player->speed);
			player->SetOrientation(glm::angleAxis(4.7f, camera_.GetSide()) * camera_.GetOrientation());
			player->Translate(camera_.GetForward()*player->speed);

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
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
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


		if (key == GLFW_KEY_1) {
			game->zoom++;
		}
		if (key == GLFW_KEY_2) {
			game->zoom--;
		}

		if (key == GLFW_KEY_E) {
			game->CreateBullet(1, game->player->GetPosition(), game->camera_.GetForward());
		}

		if (key == GLFW_KEY_T) {
			game->CreateEnemy(1, game->player->GetPosition()+glm::vec3(10,0,0));
		}

		if (key == GLFW_KEY_Z) {
			game->player->SetTexture(game->resman_.GetResource("LOghan"));
		}

		if (key == GLFW_KEY_X) {
			game->player->SetTexture(game->resman_.GetResource("BOrdy"));
		}

		if (key == GLFW_KEY_C) {
			game->player->SetTexture(game->resman_.GetResource("Grass"));
		}

		if (key == GLFW_KEY_V) {
			game->player->SetTexture(game->resman_.GetResource("ShinyTextureMaterial"));
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
	//scene_.AddNode(player);


	player->Scale(glm::vec3(1.0, 2.0, 1.0));
	glm::quat rotation = glm::angleAxis(-glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0));
	player->Rotate(rotation);
	player->SetPosition(glm::vec3(0.0, 30.0, -2.0));


	t_blade = CreateInstance("t_blade", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	t_blade->SetOrientation(glm::angleAxis(0.0f, camera_.GetForward()) * camera_.GetOrientation());
	t_blade->SetPosition(glm::vec3(0.0, 0.5, 0.5));
	t_blade->Scale(glm::vec3(4.0, 0.25, 0.01));


	tail = CreateInstance("tail", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	tail->Scale(glm::vec3(0.25, 1.5, 0.25));
	tail->SetPosition(glm::vec3(0.0, -1.5, 0.2));

	wings = CreateInstance("wings", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	wings->Scale(glm::vec3(1.8, 0.4, 0.1));
	wings->SetPosition(glm::vec3(0.0, 0.25, -0.15));

	b_blade = CreateInstance("b_blade", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	b_blade->Scale(glm::vec3(0.01, 0.1, 1.0));
	b_blade->SetPosition(glm::vec3(-0.1, -0.7, 0.0));

	SceneNode *body = CreateInstance("hellBody", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	body->Scale(glm::vec3(1.01, 1.99, 1.01));
	body->SetPosition(glm::vec3(0.0, -0.01, 0.0));


	player->AddChild(body);
    player->AddChild(t_blade);
	player->AddChild(tail);
	player->AddChild(wings);
	tail->AddChild(b_blade);

	return player;


}


void Game::CreateBullet(int num_bullets, glm::vec3 position, glm::vec3 velocity) {
	// Create instance name
	std::stringstream ss;
	ss << num_bullets;
	std::string index = ss.str();
	std::string name = "BulletInstance" + index;


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
	Bullet *bul = new Bullet(name, geom, mat, tex);


	//scene_.AddNode(bul);


	// Set attributes of bullet

	float speed = 5;

	bul->SetPosition(position + velocity * speed);
	//bul->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));
	bul->SetVelocity(velocity * speed);

}



void Game::CreateEnemy(int num_enemies, glm::vec3 position) {
	// Create instance name
	std::stringstream ss;
	ss << num_enemies;
	std::string index = ss.str();
	std::string name = "EnemyInstance" + index;


	// Get resources
	Resource *geom = resman_.GetResource("GroundMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ShinyTextureMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ShinyTextureMaterial" + std::string("\"")));
	}

	Resource *tex = resman_.GetResource("BOrdy");

	// Create asteroid instance
	Enemy *enemy = new Enemy(name, geom, mat, tex);


	//scene_.AddNode(enemy);

	MakeTank(enemy);




	// Set attributes of bullet

	float speed = 5;

	enemy->SetPosition(position);
	//bul->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));
	
	world->AddChild(enemy);
	

}

void Game::MakeTank(SceneNode* enemy) {
	enemy->Scale(glm::vec3(1.5, 3.0, 0.5));
	enemy->SetPosition(glm::vec3(0.0, 0.0, 0.0));
	glm::quat rotation = glm::angleAxis(-glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0));
	enemy->Rotate(rotation);

	SceneNode *wheel1 = CreateInstance("wheel1", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	wheel1->Scale(glm::vec3(0.5, 2.8, 0.6));
	wheel1->SetPosition(glm::vec3(0.99, 0.0, -0.25));

	SceneNode *wheel2 = CreateInstance("wheel2", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	wheel2->Scale(glm::vec3(0.5, 2.8, 0.6));
	wheel2->SetPosition(glm::vec3(-0.99, 0.0, -0.25));

	SceneNode *gun_roof = CreateInstance("gun_roof", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	gun_roof->Scale(glm::vec3(1.5, 2.15, 0.75));
	gun_roof->SetPosition(glm::vec3(0.0, -0.42, 0.61));

	SceneNode *track_cover = CreateInstance("track_cover", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	track_cover->Scale(glm::vec3(2.5, 2.9, 0.1));
	track_cover->SetPosition(glm::vec3(0.0, 0.0, 0.22));

	SceneNode *gun = CreateInstance("gun", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	rotation = glm::angleAxis(-glm::pi<float>() / -5.0f, glm::vec3(1.0, 0.0, 0.0));
	gun->Rotate(rotation);
	gun->Scale(glm::vec3(0.25, 2.25, 0.25));
	gun->SetPosition(glm::vec3(0.0, 0.75, 0.5));

	SceneNode *body = CreateInstance("body", "GroundMesh", "ShinyTextureMaterial", "BOrdy");
	body->Scale(glm::vec3(1.55, 3.0, 0.55));
	body->SetPosition(glm::vec3(0.0, -0.01, 0.0));

	enemy->AddChild(wheel1);
	enemy->AddChild(wheel2);
	enemy->AddChild(gun_roof);
	enemy->AddChild(track_cover);
	enemy->AddChild(body);
	enemy->AddChild(gun);
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

void Game::GameOver() {
	//scene_.RemoveNode(player);


}

} // namespace game


