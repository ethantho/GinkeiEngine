#pragma once

#include "lua.hpp"
#include "LuaBridge.h"
#include <memory>
#include <UnorderedMap.h>

class Rigidbody;
class ParticleSystem;

class ComponentDB
{
public:

	static inline lua_State* lua_state;
	
	static void InitializeState();
	static void InitializeOther();
	static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);
	static luabridge::LuaRef MakeRigidbody();
	static luabridge::LuaRef MakeRigidbodyCopy(Rigidbody* original);
	static luabridge::LuaRef MakeParticleSystem();
	static luabridge::LuaRef MakeParticleSystemCopy(ParticleSystem* original);
	static std::shared_ptr<luabridge::LuaRef> GetComponentType(std::string name);

	static void CppLog(const std::string& message);
	static void CppLogError(const std::string& message);
	
private:

	
	static void InitializeFunctions();
	static void InitializeComponentTypes();
	static inline std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> component_types;




};

