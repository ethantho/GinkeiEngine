#pragma once
#include "box2d/box2d.h"
#include "lua.hpp"
#include "LuaBridge.h"

class Actor;

struct HitResult {
	Actor* actor;
	b2Vec2 point;
	b2Vec2 normal;
	bool is_trigger;
};

class ClosestRaycastCallback : public b2RayCastCallback {
public:
	HitResult closest_hit_result;
	float closest_fraction = 1.0f;
	bool hit = false;

    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
		Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
		if (actor == nullptr) return closest_fraction;

		if (fraction < closest_fraction) {
			closest_fraction = fraction;
			closest_hit_result = { actor, point, normal, fixture->IsSensor()};
			if (closest_hit_result.actor == nullptr) assert(false);
			hit = true;
		}
		return closest_fraction;
    }
};

class AllRaycastCallback : public b2RayCastCallback {
public:
	std::vector<std::pair<float, HitResult>> hit_results;

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {

		Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
		if (actor == nullptr) return -1;

		HitResult result = { actor, point, normal, fixture->IsSensor() };
		//float distance = 
		hit_results.emplace_back(std::make_pair(fraction, result));

		return 1;
	}
};

class PhysicsAPI
{
public:
	static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float dist);
	static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist);

};

