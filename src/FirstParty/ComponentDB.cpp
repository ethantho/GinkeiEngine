#include "ComponentDB.h"
#include "Rigidbody.h"
#include "ParticleSystem.h"
#include <filesystem>

void ComponentDB::InitializeOther()
{
	InitializeFunctions();
	InitializeComponentTypes();
}

void ComponentDB::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table)
{
    luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
    new_metatable["__index"] = parent_table;

    instance_table.push(lua_state);
    new_metatable.push(lua_state);
    lua_setmetatable(lua_state, -2);
    lua_pop(lua_state, 1);
}

luabridge::LuaRef ComponentDB::MakeRigidbody()
{
    if (!Rigidbody::world_initialized) {
        Rigidbody::InitializeWorld();
        Rigidbody::world_initialized = true;
    }

    Rigidbody* new_component = new Rigidbody();

    return luabridge::LuaRef(lua_state, new_component);
}

luabridge::LuaRef ComponentDB::MakeRigidbodyCopy(Rigidbody* original)
{
    if (!Rigidbody::world_initialized) {
        Rigidbody::InitializeWorld();
        Rigidbody::world_initialized = true;
    }

    Rigidbody* new_component = new Rigidbody(*original);

    return luabridge::LuaRef(lua_state, new_component);
}

luabridge::LuaRef ComponentDB::MakeParticleSystem()
{
    ParticleSystem* new_component = new ParticleSystem();

    return luabridge::LuaRef(lua_state, new_component);
}

luabridge::LuaRef ComponentDB::MakeParticleSystemCopy(ParticleSystem* original)
{
    ParticleSystem* new_component = new ParticleSystem(*original);

    return luabridge::LuaRef(lua_state, new_component);
}

//only used for lua components, not c++ ones
std::shared_ptr<luabridge::LuaRef> ComponentDB::GetComponentType(std::string name)
{
    auto itr = component_types.find(name);
    if (itr == component_types.end()) {
        std::cout << "error: failed to locate component " << name;
        exit(0);
    }
    return itr->second;
}

void ComponentDB::CppLog(const std::string& message)
{
    std::cout << message << std::endl;
}

void ComponentDB::CppLogError(const std::string& message)
{
    std::cout << message << std::endl;
}

void ComponentDB::InitializeState()
{
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
}

void ComponentDB::InitializeFunctions()
{
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Debug")
        .addFunction("Log", CppLog)
        .addFunction("LogError", CppLogError)
        .endNamespace();



}

void ComponentDB::InitializeComponentTypes()
{
	std::string component_dir = "resources/component_types/";

    if (std::filesystem::exists(component_dir)) {
        for (const auto& file : std::filesystem::directory_iterator(component_dir)) {
            if (file.is_regular_file() && file.path().extension() == ".lua") {

                if (luaL_dofile(lua_state, file.path().string().c_str()) != LUA_OK) {
                    std::cout << "problem with lua file " << file.path().stem().string();
                    exit(0);
                }

                std::string component_name = file.path().stem().string();
                //std::cout << component_name << std::endl;
                component_types.insert({ component_name, std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(lua_state, component_name.c_str())) });
            }
        }
    }
    

    Rigidbody rb;
    luabridge::LuaRef rb_template(lua_state, rb);
    component_types.insert({"Rigidbody", std::make_shared<luabridge::LuaRef>(rb_template)});

    ParticleSystem ps;
    luabridge::LuaRef ps_template(lua_state, ps);
    component_types.insert({ "ParticleSystem", std::make_shared<luabridge::LuaRef>(ps_template)});
}
