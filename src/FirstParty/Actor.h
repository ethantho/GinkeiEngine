#pragma once
#include "Collision.h"
#include "ComponentDB.h"
#include "lua.hpp"
#include "LuaBridge.h"
#include "rapidjson/document.h"
#include "SDL_image.h"
#include <map>
#include <string>


extern int g_uuid;

class Rigidbody;
class ParticleSystem;

class Actor
{
public:
	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components; //does order matter? Maybe change to unordered_map
	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components_added_this_frame;
	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components_removed_this_frame;
	std::string name = "";
	//SDL_Rect* src_rect = NULL;

	int id = -1;
	bool dont_destroy = false;

	bool HasComponent(std::string component_name);

	void Update();

	void Start();

	void LateUpdate();

	void OnDestroy();

	void OnCollisionEnter(Collision collision);
	void OnCollisionExit(Collision collision);

	void OnTriggerEnter(Collision collision);
	void OnTriggerExit(Collision collision);

	void ReportError(const luabridge::LuaException& e);

	std::string GetName();
	int GetID();
	luabridge::LuaRef GetComponentByKey(std::string key);
	luabridge::LuaRef GetComponent(std::string type_name);
	luabridge::LuaRef GetComponents(std::string type_name);

	luabridge::LuaRef AddComponent(std::string type_name);
	void RemoveComponent(luabridge::LuaRef component_ref);

	void ApplyOverrides(const rapidjson::Value& actor_json);

	Actor() {}

	//used for templates, and scene actors that don't use a template
	Actor(const rapidjson::Value& actor, bool is_template = false) {

		if (!is_template) {
			id = g_uuid;
			g_uuid++;
		}

		ApplyOverrides(actor);

	}

	//used for scene actors that use a template
	Actor(const Actor& template_actor, const rapidjson::Value& actor) {
		name = template_actor.name;

		for (auto& template_component : template_actor.components) {
			luabridge::LuaRef template_component_table = *(template_component.second);

			std::string component_name = template_component_table["key"];
			luabridge::LuaRef component_table = luabridge::newTable(ComponentDB::lua_state);

			ComponentDB::EstablishInheritance(component_table, template_component_table);

			components[component_name] = std::make_shared<luabridge::LuaRef>(component_table);
		}

		id = g_uuid;
		g_uuid++;

		ApplyOverrides(actor);

	}

	//for runtime actor instantiation
	Actor(const Actor& template_actor) {
		name = template_actor.name;

		for (auto& template_component : template_actor.components) {
			luabridge::LuaRef template_component_table = *(template_component.second);

			std::string component_name = template_component_table["key"];

			std::string component_type = template_component_table["type_name"];

			luabridge::LuaRef component_table = luabridge::newTable(ComponentDB::lua_state);
			if (component_type == "Rigidbody") {
				Rigidbody* new_component = nullptr;

				Rigidbody* parent_component = template_component_table.cast<Rigidbody*>();
				new_component = ComponentDB::MakeRigidbodyCopy(parent_component);

				component_table = luabridge::LuaRef(ComponentDB::lua_state, new_component);
			}
			else if (component_type == "ParticleSystem") {
				ParticleSystem* new_component = nullptr;

				ParticleSystem* parent_component = template_component_table.cast<ParticleSystem*>();
				new_component = ComponentDB::MakeParticleSystemCopy(parent_component);

				component_table = luabridge::LuaRef(ComponentDB::lua_state, new_component);
			}
			else {
				ComponentDB::EstablishInheritance(component_table, template_component_table);
			}
			
			components[component_name] = std::make_shared<luabridge::LuaRef>(component_table);
		}

		id = g_uuid;
		g_uuid++;
	}

	~Actor() {
		OnDestroy();
	}

};

