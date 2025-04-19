#include "ComponentDB.h"
#include "PhysicsAPI.h"
#include "Rigidbody.h"
#include <algorithm>

luabridge::LuaRef PhysicsAPI::Raycast(b2Vec2 pos, b2Vec2 dir, float dist)
{
	if(dist <= 0) return luabridge::LuaRef(ComponentDB::lua_state);
	if(Rigidbody::b2world->GetBodyCount() == 0) return luabridge::LuaRef(ComponentDB::lua_state);

	ClosestRaycastCallback cb;
	Rigidbody::b2world->RayCast(&cb, pos, pos + (dist * dir));

	if(!cb.hit) return luabridge::LuaRef(ComponentDB::lua_state);

	return luabridge::LuaRef(ComponentDB::lua_state, cb.closest_hit_result);

}

luabridge::LuaRef PhysicsAPI::RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist)
{
	if (dist <= 0) return luabridge::LuaRef(ComponentDB::lua_state);
	if (Rigidbody::b2world->GetBodyCount() == 0) return luabridge::LuaRef(ComponentDB::lua_state);


	AllRaycastCallback cb;
	Rigidbody::b2world->RayCast(&cb, pos, pos + (dist * dir));

	luabridge::LuaRef ret_table = luabridge::newTable(ComponentDB::lua_state);

	std::sort(cb.hit_results.begin(), cb.hit_results.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
		});

	for (size_t i = 0; i < cb.hit_results.size(); ++i) {
		ret_table[i + 1] = cb.hit_results[i].second;
	}

	return ret_table;
}
