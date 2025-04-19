#include "glm/glm.hpp"
#include "Rigidbody.h"

void Rigidbody::InitializeWorld()
{
	b2world = new b2World({0.0f, 9.8f});
	b2world->SetContactListener(new ContactListener());
	world_initialized = true;
}

b2Vec2 Rigidbody::GetPosition()
{
	if (body == nullptr) {
		return b2Vec2(x, y);
	}
	return body->GetPosition();
}

float Rigidbody::GetRotation()
{
	if (body == nullptr) {
		return rotation;
	}
	return body->GetAngle() * (180.0f / b2_pi);
}

b2Vec2 Rigidbody::GetVelocity()
{
	return body->GetLinearVelocity();
}

float Rigidbody::GetAngularVelocity()
{
	return body->GetAngularVelocity() * (180.0f / b2_pi);
}

float Rigidbody::GetGravityScale()
{
	if (body == nullptr) {
		return gravity_scale;
	}
	return body->GetGravityScale();
}

b2Vec2 Rigidbody::GetUpDirection()
{
	float angle = body != nullptr ? body->GetAngle() + (b2_pi / 2.0f) : rotation;
	b2Vec2 result = b2Vec2(glm::cos(angle + b2_pi), glm::sin(angle + b2_pi));
	result.Normalize();
	return result;
}

b2Vec2 Rigidbody::GetRightDirection()
{
	float angle = body != nullptr ? body->GetAngle() + b2_pi : rotation;
	b2Vec2 result = b2Vec2(glm::cos(angle + b2_pi), glm::sin(angle + b2_pi));
	result.Normalize();
	return result;
}

void Rigidbody::AddForce(b2Vec2 force)
{
	body->ApplyForceToCenter(force, true);
}

void Rigidbody::SetVelocity(b2Vec2 vel)
{
	body->SetLinearVelocity(vel);
}

void Rigidbody::SetPosition(b2Vec2 pos)
{
	if (body == nullptr) {
		x = pos.x;
		y = pos.y;
		return;
	}
	body->SetTransform(pos, body->GetAngle());
}

void Rigidbody::SetRotation(float degrees_clockwise)
{
	if (body == nullptr) {
		rotation = degrees_clockwise;
		return;
	}
	body->SetTransform(body->GetPosition(), degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetAngularVelocity(float degrees_clockwise)
{
	body->SetAngularVelocity(degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetGravityScale(float new_scale)
{
	if (body == nullptr) {
		gravity_scale = new_scale;
		return;
	}
	body->SetGravityScale(new_scale);
}

void Rigidbody::SetUpDirection(b2Vec2 direction)
{
	direction.Normalize();
	float new_angle_radians = glm::atan(direction.x, -direction.y);
	if (body == nullptr) {
		rotation = new_angle_radians * (180.0f / b2_pi);
		return;
	}
	body->SetTransform(body->GetPosition(), new_angle_radians);
}

void Rigidbody::SetRightDirection(b2Vec2 direction)
{
	direction.Normalize();
	float new_angle_radians = glm::atan(direction.x, -direction.y) - (b2_pi / 2.0f);
	if (body == nullptr) {
		rotation = new_angle_radians * (180.0f / b2_pi);
		return;
	}
	body->SetTransform(body->GetPosition(), new_angle_radians);
}

void Rigidbody::OnStart()
{
	b2BodyDef body_def;
	if (body_type == "dynamic") {
		body_def.type = b2_dynamicBody;
	}
	else if (body_type == "static") {
		body_def.type = b2_staticBody;
	}
	else if (body_type == "kinematic") {
		body_def.type = b2_kinematicBody;
	}
	else {
		assert(false); //SHOULDN'T REACH HERE!
	}

	body_def.bullet = precise;
	body_def.position.x = x;
	body_def.position.y = y;
	body_def.angle = rotation * (b2_pi / 180.0f);
	body_def.gravityScale = gravity_scale;
	body_def.angularDamping = angular_friction;

	body = b2world->CreateBody(&body_def);

	if (!has_collider && !has_trigger) {
		b2PolygonShape phantom_shape;
		phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);

		b2FixtureDef phantom_fixture_def;
		phantom_fixture_def.shape = &phantom_shape;
		phantom_fixture_def.density = density;

		phantom_fixture_def.isSensor = true;
		//phantom_fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor); SHOULD NOT BE DETECTED BY RAYCAST
		body->CreateFixture(&phantom_fixture_def);
	}
	else {
		
		if (has_collider) {
			b2FixtureDef fixture;
			b2PolygonShape shape_box;
			b2CircleShape shape_circle;

			if (collider_type == "box") {

				shape_box.SetAsBox(0.5f * width, 0.5f * height);
				fixture.shape = &shape_box;
			}
			else if (collider_type == "circle") {

				shape_circle.m_radius = radius;
				fixture.shape = &shape_circle;
			}

			fixture.friction = friction;
			fixture.density = density;
			fixture.restitution = bounciness;
			fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);

			fixture.isSensor = false;
			body->CreateFixture(&fixture);
		}

		if (has_trigger) {
			b2FixtureDef fixture;
			b2PolygonShape shape_box;
			b2CircleShape shape_circle;

			if (trigger_type == "box") {

				shape_box.SetAsBox(0.5f * trigger_width, 0.5f * trigger_height);
				fixture.shape = &shape_box;
			}
			else if (trigger_type == "circle") {

				shape_circle.m_radius = trigger_radius;
				fixture.shape = &shape_circle;
			}

			fixture.friction = friction;
			fixture.density = density;
			fixture.restitution = bounciness;
			fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);

			fixture.isSensor = true;
			body->CreateFixture(&fixture);
		}
	}
}

void Rigidbody::OnDestroy()
{
	b2world->DestroyBody(body);
}
