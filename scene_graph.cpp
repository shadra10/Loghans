#include <stdexcept>
#include <iostream>
#include <stack>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"

namespace game {

SceneGraph::SceneGraph(void){

    background_color_ = glm::vec3(0.0, 0.0, 0.0);
}


SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    background_color_ = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return background_color_;
}

void SceneGraph::SetRoot(SceneNode* node) {
	root_ = node;
}
 

//SceneNode *SceneGraph::CreateNode(std::string node_name, Resource *geometry, Resource *material, Resource *texture){
//
//    // Create scene node with the specified resources
//    SceneNode *scn = new SceneNode(node_name, geometry, material, texture);

    // Add node to the scene
//    node_.push_back(scn);

//    return scn;

//}


//void SceneGraph::AddNode(SceneNode *node){

  //  node_.push_back(node);
//}

//void SceneGraph::RemoveNode(SceneNode *node) {

	// Find node with the specified name
//	for (int i = 0; i < node_.size(); i++) {
//		if (node_[i] == node) {
			//node_[i] = node_[node_.size()-1];
			//node_.pop_back();
//			node_.erase(node_.begin()+(i));
//		}
//	}
	
//}


SceneNode *SceneGraph::GetNode(std::string node_name) const {

	// Find node with the specified name
	std::stack<SceneNode *> stck;
	stck.push(root_);
	while (stck.size() > 0) {
		SceneNode *current = stck.top();
		stck.pop();
		if (current->GetName() == node_name) {
			return current;
		}
		for (std::vector<SceneNode *>::const_iterator it = current->children_begin();
			it != current->children_end(); it++) {
			stck.push(*it);
		}
	}
	return NULL;
}


//std::vector<SceneNode *>::const_iterator SceneGraph::begin() const {

 //   return node_.begin();
//}


//std::vector<SceneNode *>::const_iterator SceneGraph::end() const {

 //   return node_.end();
//}


void SceneGraph::Draw(Camera *camera){

	// Clear background
	glClearColor(background_color_[0],
		background_color_[1],
		background_color_[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw all scene nodes
	// Initialize stack of nodes
	std::stack<SceneNode *> stck;
	stck.push(root_);
	// Initialize stack of transformations
	std::stack<glm::mat4> transf;
	transf.push(glm::mat4(1.0));
	// Traverse hierarchy
	while (stck.size() > 0) {
		// Get next node to be processed and pop it from the stack
		SceneNode *current = stck.top();
		stck.pop();
		// Get transformation corresponding to the parent of the next node
		glm::mat4 parent_transf = transf.top();
		transf.pop();
		// Draw node based on parent transformation
		glm::mat4 current_transf = current->Draw(camera, parent_transf);
		// Push children of the node to the stack, along with the node's
		// transformation
		for (std::vector<SceneNode *>::const_iterator it = current->children_begin();
			it != current->children_end(); it++) {
			stck.push(*it);
			transf.push(current_transf);
		}
	}
}


void SceneGraph::Update(void){

	std::stack<SceneNode *> stck;
	stck.push(root_);
	while (stck.size() > 0) {
		SceneNode *current = stck.top();
		stck.pop();
		current->Update();
		for (std::vector<SceneNode *>::const_iterator it = current->children_begin();
			it != current->children_end(); it++) {
			stck.push(*it);
		}
	}
}

} // namespace game
