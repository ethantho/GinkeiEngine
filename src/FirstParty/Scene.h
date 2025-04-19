#pragma once

#include "Actor.h"
#include <string>
#include <vector>

class Scene
{
public:
	std::vector<Actor*> actors; //in ID order
	std::vector<Actor*> new_actors_this_frame;
	std::vector<Actor*> instantiated_actors_this_frame;
	std::vector<Actor*> actors_to_remove;

	std::string name;

	Scene() {
		actors.reserve(131072); //Is this big enough?
	}

	~Scene() {
		//std::cout << "DESTROYING SCENE" << std::endl;
		for (Actor* a : actors) {
			delete a;
		}
		for (Actor* a : instantiated_actors_this_frame) {
			delete a;
		}
	}


};

