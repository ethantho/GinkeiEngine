#pragma once

#include "Actor.h"
#include "Scene.h"

class SceneDB
{
public:

	static inline bool first_scene = true;
	static Scene current_scene;
	static inline std::string scene_to_load;
	
	static void LoadInitialScene();

	static void SetSceneToLoad(std::string scene_name);
	static std::string GetCurrent();
	static void LoadScene(std::string scene_name);
	static void LoadTiledMap(std::string tile_scene_path);

	static void DontDestroy(Actor* actor);

	static luabridge::LuaRef FindActor(std::string name);
	static luabridge::LuaRef FindAllActors(std::string name);

	static luabridge::LuaRef InstantiateActor(std::string template_actor_name);

	static void DestroyActor(Actor* actor);

};

