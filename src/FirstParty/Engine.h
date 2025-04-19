#pragma once

#include "Actor.h"
#include "Renderer.h"
#include <unordered_map>

class Engine
{
	Renderer renderer;
	
	std::string choice = "";

	bool running = true;
	bool gameover = false;

	std::unordered_map<uint64_t, std::vector<Actor*>> actor_positions;

	std::vector<std::pair<std::string, Actor*>> queued_effects;
	std::vector<std::pair<int, std::string*>> dialogue_to_draw; //id, dialogue

	int cam_width = 640;
	int cam_height = 360;

	void Input();

	void Update();

	void Render();

	void UpdateActors();

	void DoInitialSetup();

	bool CheckForGameEnd();

	void InjectClasses();


public:
	void GameLoop();

	Engine() {
		actor_positions.reserve(1048576);
	}
};


