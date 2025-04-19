#pragma once

#include "lua.hpp"
#include "LuaBridge.h"


class EventBus
{

public:
	static void Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);
	static void Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);
	static void Publish(std::string event_type, luabridge::LuaRef event_object);

	static void UpdateSubscriptions();

private:
	static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> subscriptions;
	static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> added_subscriptions_this_frame;
	static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> removed_subscriptions_this_frame;
	//maps from event_type string to vector of subscriptions
	//each subscription is a component,function pair

};

