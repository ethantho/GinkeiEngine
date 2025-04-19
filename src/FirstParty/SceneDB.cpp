#include "EngineUtils.h"
#include "SceneDB.h"
#include "TemplateDB.h"


Scene SceneDB::current_scene;

void SceneDB::LoadInitialScene()
{
	std::string config_path = "resources/game.config";
	rapidjson::Document game_config;
	EngineUtils::ReadJsonFile(config_path, game_config);

	std::string initial_scene = "";
	EngineUtils::TryLoadString(game_config, "initial_scene", initial_scene);

	//was scene specified?
	if (initial_scene == "") {
		std::cout << "error: initial_scene unspecified";
		exit(0);
	}

	LoadScene(initial_scene);
}

void SceneDB::SetSceneToLoad(std::string scene_name)
{
	scene_to_load = scene_name;
}

std::string SceneDB::GetCurrent()
{
	return current_scene.name;
}

void SceneDB::LoadScene(std::string scene_name)
{
	//does scene file exist?
	std::string scene_path = "resources/scenes/" + scene_name + ".scene";
	if (!EngineUtils::VerifyDirExists(scene_path, false)) {
		std::cout << "error: scene " + scene_name + " is missing";
		exit(0); //spec doesn't say to do this?
	}

	//does scene tiled folder exist?
	std::string tiled_scene_path = "resources/scenes/" + scene_name + ".json";
	if (!EngineUtils::VerifyDirExists(tiled_scene_path, false)) {
		//std::cout << "error: scene " + scene_name + " is missing";
		//exit(0);
	}

	//get the actors to keep between scenes
	std::vector<Actor*> actors_to_keep;
	if (!first_scene) {
		for (Actor* a : current_scene.actors) {
			if (a->dont_destroy) {
				actors_to_keep.push_back(a);
			}
		}
		for (Actor* a : current_scene.instantiated_actors_this_frame) {
			if (a->dont_destroy) {
				actors_to_keep.push_back(a);
			}
		}
	}
	
	//Don't call the Start function on actors that changed scenes, only on those that loaded in the new scenes
	current_scene = Scene();
	current_scene.name = scene_name;

	for (Actor* a : actors_to_keep) {
		current_scene.actors.push_back(a);
	}

	LoadTiledMap(tiled_scene_path);

	rapidjson::Document scene_json;

	EngineUtils::ReadJsonFile(scene_path, scene_json);

	const rapidjson::Value& actors_array = scene_json["actors"];
	for (rapidjson::SizeType i = 0; i < actors_array.Size(); ++i) {
		const rapidjson::Value& actor_json = actors_array[i];
		if (actor_json.HasMember("template")) {
			std::string template_name = actor_json["template"].GetString();
			auto itr = TemplateDB::loaded_templates.find(template_name);
			//if template not found
			if ( itr == TemplateDB::loaded_templates.end()) {
				std::cout << "error: template " << template_name << " is missing";
				exit(0);
			}

			Actor* new_actor = new Actor(itr->second, actor_json);
			//ActorHelper::ApplyActorOverrides(new_actor, actor_json);
			current_scene.actors.push_back(new_actor);
			current_scene.new_actors_this_frame.push_back(current_scene.actors.back());
		}
		else {
			Actor* new_actor = new Actor(actor_json);
			//ActorHelper::ApplyActorOverrides(new_actor, actor_json);
			current_scene.actors.push_back(new_actor);
			current_scene.new_actors_this_frame.push_back(current_scene.actors.back());
		}
		

		
	}
	first_scene = false;
}

void SceneDB::LoadTiledMap(std::string tile_scene_path)
{
	rapidjson::Document scene_json;

	EngineUtils::ReadJsonFile(tile_scene_path, scene_json);

	const rapidjson::Value& layers = scene_json["layers"];
	int tile_size = scene_json["tileheight"].GetInt();
	auto tile_template = TemplateDB::loaded_templates.find("tile");

	if (tile_template == TemplateDB::loaded_templates.end()) {
		std::cout << "error: template " << "tile" << " is missing";
		exit(0);
	}

	for (rapidjson::SizeType i = 0; i < layers.Size(); ++i) { //for each layer
		const rapidjson::Value& layer_data = layers[i];

		std::string layer_type = layer_data["type"].GetString();
		//actor layer
		if (layer_type == "objectgroup") {

			for (int j = 0; j < layer_data["objects"].Size(); ++j) { //for each actor
				rapidjson::Document components_json;
				components_json.SetArray();
				rapidjson::Document actor_json;
				actor_json.SetObject();
				for (int k = 0; k < layer_data["objects"][j]["properties"].Size(); ++k) {
					std::string_view property_name = layer_data["objects"][j]["properties"][k]["name"].GetString();
					if (property_name == "components") {
						std::string view = layer_data["objects"][j]["properties"][k]["value"].GetString();
						components_json.Parse(view.c_str());
						actor_json.AddMember("components", components_json, actor_json.GetAllocator());
					}
					else if (property_name == "name") {
						rapidjson::Document::AllocatorType& allocator = actor_json.GetAllocator();
						rapidjson::Value key("name");
						std::string name = layer_data["objects"][j]["properties"][k]["value"].GetString();
						rapidjson::Value val(name.c_str(), allocator);
						actor_json.AddMember(key, val, allocator);
					}
				}
				
				//actor_json["components"] = components_json.GetObject();
				Actor* new_actor = new Actor(actor_json);

				luabridge::LuaRef rb = new_actor->GetComponent("Rigidbody");

				float x = layer_data["objects"][j]["x"].GetFloat() / 100.0f;
				float y = layer_data["objects"][j]["y"].GetFloat() / 100.0f;

				if (rb == luabridge::LuaRef(ComponentDB::lua_state)) {
					rb = new_actor->GetComponent("Transform");
					rb["position"] = glm::vec2(x, y);
				}

				luabridge::LuaRef SetPosition = rb["SetPosition"];

				SetPosition(rb, b2Vec2(x,y));

				current_scene.actors.push_back(new_actor);
				current_scene.new_actors_this_frame.push_back(current_scene.actors.back());
			}
			
			continue;
		}




		//tile layer
		int width = layer_data["width"].GetInt();
		const float tile_width_units = 0.32f; //TODO: not hard code this

		for (int j = 0; j < layer_data["data"].Size(); ++j) { //for each tile in the layer
			//rapidjson::Value dummy_json;
			//dummy_json.SetArray();
			Actor* new_tile = new Actor(tile_template->second);

			const float tile_x = static_cast<float>(j % width) * tile_width_units;
			const float tile_y = static_cast<float>(j / width) * tile_width_units;

			luabridge::LuaRef new_transform = new_tile->GetComponent("Transform");
			new_transform["position"] = glm::vec2(tile_x, tile_y);

			luabridge::LuaRef tile_renderer = new_tile->GetComponent("TileRenderer");
			tile_renderer["tile_id"] = layer_data["data"][j].GetInt() - 1;

			current_scene.actors.push_back(new_tile);
			current_scene.new_actors_this_frame.push_back(current_scene.actors.back());
		}
		
	}
}

void SceneDB::DontDestroy(Actor* actor)
{
	actor->dont_destroy = true;
}

luabridge::LuaRef SceneDB::FindActor(std::string name)
{
	for (Actor* a : current_scene.actors_to_remove) {
		if (a->name == name) {
			return luabridge::LuaRef(ComponentDB::lua_state);
			//return nil because it was marked for removal
		}
	}

	for (Actor* a : current_scene.actors) {
		if (a->name == name) {
			return luabridge::LuaRef(ComponentDB::lua_state, a);
		}
	}
	for (Actor* a : current_scene.instantiated_actors_this_frame) {
		if (a->name == name) {
			return luabridge::LuaRef(ComponentDB::lua_state, a);
		}
	}
	return luabridge::LuaRef(ComponentDB::lua_state);
}

luabridge::LuaRef SceneDB::FindAllActors(std::string name)
{
	std::vector<Actor*> ret_actors;

	for (Actor* a : current_scene.actors) {
		if (a->name == name) {
			ret_actors.push_back(a);
		}
	}
	for (Actor* a : current_scene.instantiated_actors_this_frame) {
		if (a->name == name) {
			ret_actors.push_back(a);
		}
	}

	luabridge::LuaRef ret_table = luabridge::newTable(ComponentDB::lua_state);

	for (int i = 0; i < ret_actors.size(); ++i) {
		ret_table[i + 1] = ret_actors[i];
	}

	return ret_table;
}

luabridge::LuaRef SceneDB::InstantiateActor(std::string template_actor_name)
{
	auto itr = TemplateDB::loaded_templates.find(template_actor_name);
	//if template not found
	if (itr == TemplateDB::loaded_templates.end()) {
		std::cout << "error: template " << template_actor_name << " is missing";
		exit(0);
	}

	Actor* actor = new Actor(itr->second);
	//ActorHelper::ApplyActorOverrides(actor, actor_json);

	current_scene.instantiated_actors_this_frame.emplace_back(actor);

	return luabridge::LuaRef(ComponentDB::lua_state, current_scene.instantiated_actors_this_frame.back());
}

void SceneDB::DestroyActor(Actor* actor)
{
	for (auto &c : actor->components) {
		(*c.second)["enabled"] = false;
	}
	current_scene.actors_to_remove.push_back(actor);


}


