#pragma once

#include "box2d/box2d.h"

class Actor; //For circular dependency

class Collision
{
public:
	Actor* other;
	b2Vec2 point;
	b2Vec2 relative_velocity;
	b2Vec2 normal;

	Collision(Actor* other, b2Vec2 point, b2Vec2 relative_velocity, b2Vec2 normal) {
		this->other = other;
		this->point = point;
		this->relative_velocity = relative_velocity;
		this->normal = normal;
	}
};

