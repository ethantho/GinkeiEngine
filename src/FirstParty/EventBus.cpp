#include "EventBus.h"
#include <algorithm>

void EventBus::Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function)
{
	added_subscriptions_this_frame[event_type].push_back(std::make_pair(component, function));
}

void EventBus::Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function)
{
	removed_subscriptions_this_frame[event_type].push_back(std::make_pair(component, function));
}

void EventBus::Publish(std::string event_type, luabridge::LuaRef event_object)
{
	for (auto& subscription : subscriptions[event_type]) {
		//subscription is a (component,function) pair
		if (!(subscription.first)["enabled"]) {
			continue;
		}
		try {
			if (subscription.second.isFunction()) {
				subscription.second(subscription.first, event_object);
			}
		}
		catch(luabridge::LuaException e){
			std::string error_message = e.what();
			std::replace(error_message.begin(), error_message.end(), '\\', '/');
			std::cout << "\033[31m" << (subscription.first)["actor"] << " : " << error_message << "\033[0m" << std::endl;
		}
	}
}

void EventBus::UpdateSubscriptions()
{
	for (auto& sub_vec : added_subscriptions_this_frame){
		for (auto& sub : sub_vec.second) {
			subscriptions[sub_vec.first].push_back(sub);
		}
	}
	added_subscriptions_this_frame.clear();

	for (auto& sub_vec : removed_subscriptions_this_frame) {
		for (auto& sub : sub_vec.second) {
			subscriptions[sub_vec.first].erase(std::remove(subscriptions[sub_vec.first].begin(), subscriptions[sub_vec.first].end(), sub), subscriptions[sub_vec.first].end());
		}
	}
	removed_subscriptions_this_frame.clear();

}
