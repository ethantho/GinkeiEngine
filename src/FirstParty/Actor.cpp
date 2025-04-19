#include "Actor.h"
#include "Helper.h"

extern int g_n;

bool Actor::HasComponent(std::string component_name)
{
	auto itr = components.find(component_name);
	if (itr != components.end()) {
		return true;
	}
	return false;
}

void Actor::Update()
{
	for (auto& c : components_added_this_frame) {
		if (!((*c.second)["enabled"])) {
			continue;
		}
		try {
			luabridge::LuaRef OnStartFunction = (*c.second)["OnStart"];
			if (OnStartFunction.isFunction()) {
				OnStartFunction(*c.second);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}
		components.insert(c);
	}
	components_added_this_frame.clear();

	for (auto& c : components) {
		if (!((*c.second)["enabled"])) {
			continue;
		}
		try {
			luabridge::LuaRef OnUpdateFunction = (*c.second)["OnUpdate"];
			if (OnUpdateFunction.isFunction()) {
				OnUpdateFunction(*c.second);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}

	}

	/*for (auto& c : components_removed_this_frame) { //COMMENTING THIS OUT TECHNICALLY MAKES HOMEWORK7 STUFF WRONG! DON'T WORRY ABOUT IT FOR NOW!
		components.erase(c.first);
	}
	components_removed_this_frame.clear();*/

}

//TODO: ORDER COMPONENTS BY KEY
void Actor::Start()
{
	for (auto& c_pair : components) {
		(*(c_pair.second))["actor"] = this;
	}

	for (auto& c : components) {
		if (!((*c.second)["enabled"])) {
			continue;
		}
		try {
			luabridge::LuaRef OnStartFunction = (*c.second)["OnStart"];
			if (OnStartFunction.isFunction()) {
				OnStartFunction(*c.second);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}

	}
	
	
}

//TODO: ORDER COMPONENTS BY KEY
void Actor::LateUpdate()
{
	for (auto& c : components) {
		if (!((*c.second)["enabled"])) {
			continue;
		}
		try {
			luabridge::LuaRef OnLateUpdateFunction = (*c.second)["OnLateUpdate"];
			if (OnLateUpdateFunction.isFunction()) {
				OnLateUpdateFunction(*c.second);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}

	}

	for (auto& c : components_removed_this_frame) {
		try {
			luabridge::LuaRef OnDestroyFunction = (*c.second)["OnDestroy"];
			if (OnDestroyFunction.isFunction()) {
				OnDestroyFunction(*c.second);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}
		components.erase(c.first);
	}

	components_removed_this_frame.clear();
}

void Actor::OnDestroy()
{
	for (auto& c : components) {
		try {
			luabridge::LuaRef OnDestroyFunction = (*c.second)["OnDestroy"];
			if (OnDestroyFunction.isFunction()) {
				OnDestroyFunction(*c.second);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}
	}
}

void Actor::OnCollisionEnter(Collision collision)
{
	for (auto& c : components) {
		if (!((*c.second)["enabled"])) {
			continue;
		}
		try {
			luabridge::LuaRef OnCollisionEnterFunction = (*c.second)["OnCollisionEnter"];
			if (OnCollisionEnterFunction.isFunction()) {
				OnCollisionEnterFunction(*c.second, collision);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}

	}
}

void Actor::OnCollisionExit(Collision collision)
{
	for (auto& c : components) {
		if (!((*c.second)["enabled"])) {
			continue;
		}
		try {
			luabridge::LuaRef OnCollisionExitFunction = (*c.second)["OnCollisionExit"];
			if (OnCollisionExitFunction.isFunction()) {
				OnCollisionExitFunction(*c.second, collision);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}

	}
}

void Actor::OnTriggerEnter(Collision collision)
{
	for (auto& c : components) {
		try {
			if (c.second.get() != nullptr && !((*c.second)["enabled"])) {
				continue;
			}
			luabridge::LuaRef OnTriggerEnterFunction = (*c.second)["OnTriggerEnter"];
			if (OnTriggerEnterFunction.isFunction()) {
				OnTriggerEnterFunction(*c.second, collision);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}

	}
}

void Actor::OnTriggerExit(Collision collision)
{
	for (auto& c : components) {
		try{
			if (c.second.get() != nullptr && !((*c.second)["enabled"].cast<bool>())) {
				continue;
			}
			luabridge::LuaRef OnTriggerExitFunction = (*c.second)["OnTriggerExit"];
			if (OnTriggerExitFunction.isFunction()) {
				OnTriggerExitFunction(*c.second, collision);
			}
		}
		catch (luabridge::LuaException e) {
			ReportError(e);
		}

	}
}

void Actor::ReportError(const luabridge::LuaException& e)
{
	std::string error_message = e.what();

	std::replace(error_message.begin(), error_message.end(), '\\', '/');

	std::cout << "\033[31m" << name << " : " << error_message << "\033[0m" << std::endl;
}

std::string Actor::GetName()
{
	return name;
}

int Actor::GetID()
{
	return id;
}

luabridge::LuaRef Actor::GetComponentByKey(std::string key)
{
	auto removed_itr = components_removed_this_frame.find(key);
	if(removed_itr != components_removed_this_frame.end()) return luabridge::LuaRef(ComponentDB::lua_state);
	auto itr = components.find(key);

	if (itr != components.end()) {
		return *(itr->second);
	}
	return luabridge::LuaRef(ComponentDB::lua_state);
}

luabridge::LuaRef Actor::GetComponent(std::string type_name)
{
	std::string current_first_key = "";
	std::shared_ptr<luabridge::LuaRef> result = nullptr;
	if(Helper::GetFrameNumber() >= 45){
		int i = 0; //breakpoint
	}
	for (auto cpair : components) {
		if ((*(cpair.second))["type_name"] == type_name && (current_first_key == "" || cpair.first < current_first_key)) {
			auto removed_itr = components_removed_this_frame.find(cpair.first);
			if (removed_itr == components_removed_this_frame.end()) {
				result = cpair.second;
				current_first_key = cpair.first;
			}
		}
	}
	/*if (type_name == "Rigidbody" && Helper::GetFrameNumber() >= 45) {
		return luabridge::LuaRef(ComponentDB::lua_state, result->cast<Rigidbody*>());
	}*/
	if (result != nullptr) return *result;
	return luabridge::LuaRef(ComponentDB::lua_state);
}

luabridge::LuaRef Actor::GetComponents(std::string type_name)
{
	std::vector<std::shared_ptr<luabridge::LuaRef>> ret_components;

	for (auto cpair : components) {
		if ((*(cpair.second))["type_name"] == type_name) {
			auto removed_itr = components_removed_this_frame.find(cpair.first);
			if (removed_itr == components_removed_this_frame.end()) {
				ret_components.push_back(cpair.second);
			}
		}
	}

	if(ret_components.empty()) return luabridge::LuaRef(ComponentDB::lua_state);

	std::sort(ret_components.begin(), ret_components.end(), [](std::shared_ptr<luabridge::LuaRef> a, std::shared_ptr<luabridge::LuaRef> b) {
		return (*a)["key"] < (*b)["key"];
	});

	luabridge::LuaRef ret_table = luabridge::newTable(ComponentDB::lua_state);

	for (int i = 0; i < ret_components.size(); ++i) {
		ret_table[i + 1] = *ret_components[i];
	}

	return ret_table;
}

luabridge::LuaRef Actor::AddComponent(std::string type_name)
{
	std::string component_key = "r" + std::to_string(g_n++);
	luabridge::LuaRef component_table = luabridge::newTable(ComponentDB::lua_state);

	if (type_name == "Rigidbody") {
		component_table = ComponentDB::MakeRigidbody();
		component_table = component_table.cast<Rigidbody*>();
		component_table["key"] = component_key;
		component_table["enabled"] = true;
	}
	else if (type_name == "ParticleSystem") {
		component_table = ComponentDB::MakeParticleSystem();
		component_table = component_table.cast<ParticleSystem*>();
		component_table["key"] = component_key;
		component_table["enabled"] = true;
	}
	else {
		ComponentDB::EstablishInheritance(component_table, *ComponentDB::GetComponentType(type_name));
		component_table["key"] = component_key;
		component_table["enabled"] = true;
		component_table["type_name"] = type_name;
	}

	components_added_this_frame[component_key] = std::make_shared<luabridge::LuaRef>(component_table);
	
	return component_table;	
}

void Actor::RemoveComponent(luabridge::LuaRef component_ref)
{
	component_ref["enabled"] = false;
	components_removed_this_frame[component_ref["key"]] = std::make_shared<luabridge::LuaRef>(component_ref);
}

void Actor::ApplyOverrides(const rapidjson::Value& actor_json)
{
	//if (!actor_json.IsArray()) return;
	for (auto it = actor_json.MemberBegin(); it != actor_json.MemberEnd(); ++it) {
		std::string member_name = it->name.GetString();
		const rapidjson::Value& value = it->value;

		if (value.IsString()) { //Cheating these with single character comparisons
			if (member_name == "name") {
				name = value.GetString();
			}
		}
		else if (value.IsObject()) {
			if (member_name == "components") {
				for (auto& c : value.GetObject()) { //for each component

					std::string component_key = c.name.GetString();

					//if already has this component
					if (HasComponent(component_key)) {
						luabridge::LuaRef component_table = *(components)[component_key];
						for (auto& attribute : c.value.GetObject()) { //for each attribute of the component listed in json

							std::string attribute_name = attribute.name.GetString();

							if (attribute.value.IsString()) {
								component_table[attribute.name.GetString()] = attribute.value.GetString();
							}
							else if (attribute.value.IsFloat()) {
								component_table[attribute.name.GetString()] = attribute.value.GetFloat();
							}
							else if (attribute.value.IsInt()) {
								component_table[attribute.name.GetString()] = attribute.value.GetInt();
							}
							else if (attribute.value.IsBool()) {
								component_table[attribute.name.GetString()] = attribute.value.GetBool();
							}

						}
					}
					else { //if doesn't already have this component
						luabridge::LuaRef component_table = luabridge::newTable(ComponentDB::lua_state);

						for (auto& attribute : c.value.GetObject()) { //for each attribute of the component

							std::string attribute_name = attribute.name.GetString();
							std::string debug_check = "";
							if (attribute.value.IsString()) {
								debug_check = attribute.value.GetString();
							}

							if (attribute.name == "type") {
								if (attribute.value == "Rigidbody") {
									component_table = ComponentDB::MakeRigidbody();
								}
								else if (attribute.value == "ParticleSystem") {
									component_table = ComponentDB::MakeParticleSystem();
								}
								else {
									ComponentDB::EstablishInheritance(component_table, *ComponentDB::GetComponentType(attribute.value.GetString()));
									component_table["type_name"] = attribute.value.GetString();
								}
							}
							else {
								if (attribute.value.IsString()) {
									component_table[attribute.name.GetString()] = attribute.value.GetString();
								}
								else if (attribute.value.IsFloat()) {
									component_table[attribute.name.GetString()] = attribute.value.GetFloat();
								}
								else if (attribute.value.IsInt()) {
									component_table[attribute.name.GetString()] = attribute.value.GetInt();
								}
								else if (attribute.value.IsBool()) {
									component_table[attribute.name.GetString()] = attribute.value.GetBool();
								}
							}
						}

						component_table["key"] = component_key;

						component_table["enabled"] = true;

						components[component_key] = std::make_shared<luabridge::LuaRef>(component_table);
					}
				}
			}
		}
	}
}
