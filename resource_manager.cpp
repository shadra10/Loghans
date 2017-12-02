#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SOIL/SOIL.h>

#include "resource_manager.h"

namespace game {

ResourceManager::ResourceManager(void){
}


ResourceManager::~ResourceManager(){
}


void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size){

    Resource *res;

    res = new Resource(type, name, resource, size);

    resource_.push_back(res);
}


void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size){

    Resource *res;

    res = new Resource(type, name, array_buffer, element_array_buffer, size);

    resource_.push_back(res);
}


void ResourceManager::LoadResource(ResourceType type, const std::string name, const char *filename){

    // Call appropriate method depending on type of resource
    if (type == Material){
        LoadMaterial(name, filename);
    } else if (type == Texture){
        LoadTexture(name, filename);
    } else {
        throw(std::invalid_argument(std::string("Invalid type of resource")));
    }
}


Resource *ResourceManager::GetResource(const std::string name) const {

    // Find resource with the specified name
    for (int i = 0; i < resource_.size(); i++){
        if (resource_[i]->GetName() == name){
            return resource_[i];
        }
    }
    return NULL;
}


void ResourceManager::LoadMaterial(const std::string name, const char *prefix){

    // Load vertex program source code
    std::string filename = std::string(prefix) + std::string(VERTEX_PROGRAM_EXTENSION);
    std::string vp = LoadTextFile(filename.c_str());

    // Load fragment program source code
    filename = std::string(prefix) + std::string(FRAGMENT_PROGRAM_EXTENSION);
    std::string fp = LoadTextFile(filename.c_str());

    // Create a shader from the vertex program source code
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *source_vp = vp.c_str();
    glShaderSource(vs, 1, &source_vp, NULL);
    glCompileShader(vs);

    // Check if shader compiled successfully
    GLint status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(vs, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error compiling vertex shader: ")+std::string(buffer)));
    }

    // Create a shader from the fragment program source code
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *source_fp = fp.c_str();
    glShaderSource(fs, 1, &source_fp, NULL);
    glCompileShader(fs);

    // Check if shader compiled successfully
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(fs, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error compiling fragment shader: ")+std::string(buffer)));
    }

    // Create a shader program linking both vertex and fragment shaders
    // together
    GLuint sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    // Check if shaders were linked successfully
    glGetProgramiv(sp, GL_LINK_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(sp, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error linking shaders: ")+std::string(buffer)));
    }

    // Delete memory used by shaders, since they were already compiled
    // and linked
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Add a resource for the shader program
    AddResource(Material, name, sp, 0);
}


std::string ResourceManager::LoadTextFile(const char *filename){

    // Open file
    std::ifstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }

    // Read file
    std::string content;
    std::string line;
    while(std::getline(f, line)){
        content += line + "\n";
    }

    // Close file
    f.close();

    return content;
}


void ResourceManager::CreateTorus(std::string object_name, float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples){

    // Create a torus
    // The torus is built from a large loop with small circles around the loop

    // Number of vertices and faces to be created
    // Check the construction algorithm below to understand the numbers
    // specified below
    const GLuint vertex_num = num_loop_samples*num_circle_samples;
    const GLuint face_num = num_loop_samples*num_circle_samples*2;

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Data buffers for the torus
    GLfloat *vertex = NULL;
    GLuint *face = NULL;

    // Allocate memory for buffers
    try {
        vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        face = new GLuint[face_num * face_att]; // 3 indices per face
    }
    catch  (std::exception &e){
        throw e;
    }

    // Create vertices 
    float theta, phi; // Angles for circles
    glm::vec3 loop_center;
    glm::vec3 vertex_position;
    glm::vec3 vertex_normal;
    glm::vec3 vertex_color;
    glm::vec2 vertex_coord;

    for (int i = 0; i < num_loop_samples; i++){ // large loop
        
        theta = 2.0*glm::pi<GLfloat>()*i/num_loop_samples; // loop sample (angle theta)
        loop_center = glm::vec3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

        for (int j = 0; j < num_circle_samples; j++){ // small circle
            
            phi = 2.0*glm::pi<GLfloat>()*j/num_circle_samples; // circle sample (angle phi)
            
            // Define position, normal and color of vertex
            vertex_normal = glm::vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
            vertex_position = loop_center + vertex_normal*circle_radius;
            vertex_color = glm::vec3(1.0 - ((float) i / (float) num_loop_samples), 
                                            (float) i / (float) num_loop_samples, 
                                            (float) j / (float) num_circle_samples);
            vertex_coord = glm::vec2(theta / 2.0*glm::pi<GLfloat>(),
                                     phi / 2.0*glm::pi<GLfloat>());

            // Add vectors to the data buffer
            for (int k = 0; k < 3; k++){
                vertex[(i*num_circle_samples+j)*vertex_att + k] = vertex_position[k];
                vertex[(i*num_circle_samples+j)*vertex_att + k + 3] = vertex_normal[k];
                vertex[(i*num_circle_samples+j)*vertex_att + k + 6] = vertex_color[k];
            }
            vertex[(i*num_circle_samples+j)*vertex_att + 9] = vertex_coord[0];
            vertex[(i*num_circle_samples+j)*vertex_att + 10] = vertex_coord[1];
        }
    }

    // Create triangles
    for (int i = 0; i < num_loop_samples; i++){
        for (int j = 0; j < num_circle_samples; j++){
            // Two triangles per quad
            glm::vec3 t1(((i + 1) % num_loop_samples)*num_circle_samples + j, 
                         i*num_circle_samples + ((j + 1) % num_circle_samples),
                         i*num_circle_samples + j);    
            glm::vec3 t2(((i + 1) % num_loop_samples)*num_circle_samples + j,
                         ((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
                         i*num_circle_samples + ((j + 1) % num_circle_samples));
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*num_circle_samples+j)*face_att*2 + k] = (GLuint) t1[k];
                face[(i*num_circle_samples+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
            }
        }
    }

    // Create OpenGL buffers and copy data
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    // Free data buffers
    delete [] vertex;
    delete [] face;

    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}


void ResourceManager::CreateSphere(std::string object_name, float radius, int num_samples_theta, int num_samples_phi){

    // Create a sphere using a well-known parameterization

    // Number of vertices and faces to be created
    const GLuint vertex_num = num_samples_theta*num_samples_phi;
    const GLuint face_num = num_samples_theta*(num_samples_phi-1)*2;

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Data buffers 
    GLfloat *vertex = NULL;
    GLuint *face = NULL;

    // Allocate memory for buffers
    try {
        vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        face = new GLuint[face_num * face_att]; // 3 indices per face
    }
    catch  (std::exception &e){
        throw e;
    }

    // Create vertices 
    float theta, phi; // Angles for parametric equation
    glm::vec3 vertex_position;
    glm::vec3 vertex_normal;
    glm::vec3 vertex_color;
    glm::vec2 vertex_coord;
   
    for (int i = 0; i < num_samples_theta; i++){
            
        theta = 2.0*glm::pi<GLfloat>()*i/(num_samples_theta-1); // angle theta
            
        for (int j = 0; j < num_samples_phi; j++){
                    
            phi = glm::pi<GLfloat>()*j/(num_samples_phi-1); // angle phi

            // Define position, normal and color of vertex
            vertex_normal = glm::vec3(cos(theta)*sin(phi), sin(theta)*sin(phi), -cos(phi));
            // We need z = -cos(phi) to make sure that the z coordinate runs from -1 to 1 as phi runs from 0 to pi
            // Otherwise, the normal will be inverted
            vertex_position = glm::vec3(vertex_normal.x*radius, 
                                        vertex_normal.y*radius, 
                                        vertex_normal.z*radius),
            vertex_color = glm::vec3(((float)i)/((float)num_samples_theta), 1.0-((float)j)/((float)num_samples_phi), ((float)j)/((float)num_samples_phi));
            vertex_coord = glm::vec2(((float)i)/((float)num_samples_theta), 1.0-((float)j)/((float)num_samples_phi));

            // Add vectors to the data buffer
            for (int k = 0; k < 3; k++){
                vertex[(i*num_samples_phi+j)*vertex_att + k] = vertex_position[k];
                vertex[(i*num_samples_phi+j)*vertex_att + k + 3] = vertex_normal[k];
                vertex[(i*num_samples_phi+j)*vertex_att + k + 6] = vertex_color[k];
            }
            vertex[(i*num_samples_phi+j)*vertex_att + 9] = vertex_coord[0];
            vertex[(i*num_samples_phi+j)*vertex_att + 10] = vertex_coord[1];
        }
    }

    // Create faces
    for (int i = 0; i < num_samples_theta; i++){
        for (int j = 0; j < (num_samples_phi-1); j++){
            // Two triangles per quad
            glm::vec3 t1(((i + 1) % num_samples_theta)*num_samples_phi + j, 
                         i*num_samples_phi + (j + 1),
                         i*num_samples_phi + j);
            glm::vec3 t2(((i + 1) % num_samples_theta)*num_samples_phi + j, 
                         ((i + 1) % num_samples_theta)*num_samples_phi + (j + 1), 
                         i*num_samples_phi + (j + 1));
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*(num_samples_phi-1)+j)*face_att*2 + k] = (GLuint) t1[k];
                face[(i*(num_samples_phi-1)+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
            }
        }
    }

    // Create OpenGL buffers and copy data
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    // Free data buffers
    delete [] vertex;
    delete [] face;

    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}




void ResourceManager::CreateCube(std::string object_name) {
	GLfloat cubeVertices[]  = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};


	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	std::vector<GLuint> indices;

	for (int i = 0; i < 36; i++)
		indices.push_back(i);


	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, &indices[0], GL_STATIC_DRAW);

	AddResource(Mesh, object_name, vbo, ebo, indices.size());
}

void ResourceManager::CreateGround(std::string name) {
	GLfloat cubeVertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 1000.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 1000.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1000.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 000.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 000.0f, 000.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 000.0f, 000.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 000.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 1000.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1000.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1000.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 1000.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1000.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1000.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1000.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 1000.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1000.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1000.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 50.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 50.0f, 50.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 50.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 50.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 50.0f
	};


	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	std::vector<GLuint> indices;

	for (int i = 0; i < 36; i++)
		indices.push_back(i);


	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, &indices[0], GL_STATIC_DRAW);

	AddResource(Mesh, name, vbo, ebo, indices.size());

}



void ResourceManager::LoadTexture(const std::string name, const char *filename){

    // Load texture from file
    GLuint texture = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
    if (!texture){
        throw(std::ios_base::failure(std::string("Error loading texture ")+std::string(filename)+std::string(": ")+std::string(SOIL_last_result())));
    }

    // Create resource
    AddResource(Texture, name, texture, 0);
}

} // namespace game;
