#pragma once

#include "Actor.h"
#include "box2d/box2d.h"
#include "Collision.h"
#include <string>

class ContactListener : public b2ContactListener {
	void BeginContact(b2Contact* contact) {
		b2Fixture* a = contact->GetFixtureA();
		b2Fixture* b = contact->GetFixtureB();

		if (a->IsSensor() != b->IsSensor()) return; //Make sure to ignore trigger/collider collisions

		Actor* actor1 = reinterpret_cast<Actor*>(a->GetUserData().pointer);
		Actor* actor2 = reinterpret_cast<Actor*>(b->GetUserData().pointer);

		if (actor1 == nullptr || actor2 == nullptr) return;

		b2WorldManifold wm;
		contact->GetWorldManifold(&wm);

		b2Vec2 relative_velocity = a->GetBody()->GetLinearVelocity() - b->GetBody()->GetLinearVelocity();

		if (a->IsSensor()) {
			Collision c1(actor2, { -999.0f, -999.0f }, relative_velocity, { -999.0f, -999.0f });
			Collision c2(actor1, { -999.0f, -999.0f }, relative_velocity, { -999.0f, -999.0f });
			actor1->OnTriggerEnter(c1);
			actor2->OnTriggerEnter(c2);
		}
		else {
			Collision c1(actor2, wm.points[0], relative_velocity, wm.normal);
			Collision c2(actor1, wm.points[0], relative_velocity, wm.normal);
			actor1->OnCollisionEnter(c1);
			actor2->OnCollisionEnter(c2);
		}
		
	}

	void EndContact(b2Contact* contact) {
		b2Fixture* a = contact->GetFixtureA();
		b2Fixture* b = contact->GetFixtureB();

		if (a->IsSensor() != b->IsSensor()) return; //Make sure to ignore trigger/collider collisions

		Actor* actor1 = reinterpret_cast<Actor*>(a->GetUserData().pointer);
		Actor* actor2 = reinterpret_cast<Actor*>(b->GetUserData().pointer);

		if (actor1 == nullptr || actor2 == nullptr) return;

		b2WorldManifold wm;
		contact->GetWorldManifold(&wm);

		b2Vec2 relative_velocity = a->GetBody()->GetLinearVelocity() - b->GetBody()->GetLinearVelocity();

		if (a->IsSensor()) {
			Collision c1(actor2, { -999.0f, -999.0f }, relative_velocity, { -999.0f, -999.0f });
			Collision c2(actor1, { -999.0f, -999.0f }, relative_velocity, { -999.0f, -999.0f });
			actor1->OnTriggerExit(c1);
			actor2->OnTriggerExit(c2);
		}
		else {
			Collision c1(actor2, { -999.0f, -999.0f }, relative_velocity, { -999.0f, -999.0f });
			Collision c2(actor1, { -999.0f, -999.0f }, relative_velocity, { -999.0f, -999.0f });
			actor1->OnCollisionExit(c1);
			actor2->OnCollisionExit(c2);
		}
	}
};

class Rigidbody
{
public:
	static inline bool world_initialized = false;
	static inline b2World* b2world = nullptr;

	static void InitializeWorld();

	std::string type_name = "Rigidbody";
	std::string key = "???";
	Actor* actor = nullptr;
	bool enabled = true;

	float x = 0.0f;
	float y = 0.0f;
	std::string body_type = "dynamic";
	bool precise = true;
	float gravity_scale = 1.0f;
	float density = 1.0f;
	float angular_friction = 0.3f;
	float rotation = 0.0f; //WE USE DEGREES, BOX2D USES RADIANS
	bool has_collider = true;
	bool has_trigger = true;

	std::string collider_type = "box";
	std::string trigger_type = "box";

	float width = 1.0f;
	float height = 1.0f;
	float radius = 0.5f;
	float friction = 0.3f;
	float bounciness = 0.3f;

	float trigger_width = 1.0f;
	float trigger_height = 1.0f;
	float trigger_radius = 0.5f;

	b2Body* body;

	b2Vec2 GetPosition();
	float GetRotation();
	b2Vec2 GetVelocity();
	float GetAngularVelocity();
	float GetGravityScale();
	b2Vec2 GetUpDirection();
	b2Vec2 GetRightDirection();

	void AddForce(b2Vec2 force);
	void SetVelocity(b2Vec2 vel);
	void SetPosition(b2Vec2 pos);
	void SetRotation(float degrees_clockwise);
	void SetAngularVelocity(float degrees_clockwise);
	void SetGravityScale(float new_scale);
	void SetUpDirection(b2Vec2 direction);
	void SetRightDirection(b2Vec2 direction);

	void OnStart();
	void OnDestroy();
};

